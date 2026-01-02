/**************************************************************************************************/
/**
 * @file database_manager.cpp
 * @brief Implementation of DatabaseManager
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * For commercial licensing options, please contact: tdiego001@gmail.com
 */
/**************************************************************************************************/

#include "database_manager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QVariant>

#include "utils/utils_logger.h"

#include <chrono>
#include <mutex>
#include <stdexcept>

namespace Orogena::Database
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

std::unique_ptr<DatabaseManager> DatabaseManager::s_Instance = nullptr;
std::mutex                       DatabaseManager::s_InstanceMutex;

//=================================================================================================
// PooledConnectionGuard Implementation
//=================================================================================================

PooledConnectionGuard::PooledConnectionGuard(DatabaseManager&   manager,
                                             const std::string& connectionName)
    : m_Manager(manager), m_ConnectionName(connectionName)
{
}

PooledConnectionGuard::~PooledConnectionGuard()
{
    m_Manager.ReleaseConnection(m_ConnectionName);
}

//=================================================================================================
// DatabaseManager - Singleton
//=================================================================================================

DatabaseManager& DatabaseManager::Instance()
{
    std::lock_guard<std::mutex> lock(s_InstanceMutex);

    if (!s_Instance)
    {
        s_Instance.reset(new DatabaseManager());
    }

    return *s_Instance;
}

void DatabaseManager::DestroyInstance()
{
    std::lock_guard<std::mutex> lock(s_InstanceMutex);
    s_Instance.reset();
}

//=================================================================================================
// Constructor/Destructor
//=================================================================================================

DatabaseManager::DatabaseManager()
    : m_PoolSize(5), // Default pool size
      m_Connected(false)
{
    Log::Info("DatabaseManager initialized with pool size {}", m_PoolSize);
}

DatabaseManager::~DatabaseManager()
{
    if (m_Connected)
    {
        Disconnect();
    }
    Log::Info("DatabaseManager destroyed");
}

//=================================================================================================
// Connection Management
//=================================================================================================

bool DatabaseManager::Connect(const std::string& path)
{
    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);

        if (m_Connected)
        {
            Log::Warn("Already connected to database, disconnecting first");
            // Can't call Disconnect() here - it also needs the lock
            // Set flag to disconnect after releasing lock
        }

        m_DatabasePath = path;

        if (!InitializePool())
        {
            Log::Error("Failed to initialize connection pool");
            return false;
        }

        m_Connected = true;
    } // Release lock here

    Log::Info("Connected to database: {}", path);

    // Initialize schema without holding the pool lock
    // InitializeSchema() will acquire connections as needed
    if (!InitializeSchema())
    {
        Log::Error("Failed to initialize database schema");
        Disconnect();
        return false;
    }

    return true;
}

void DatabaseManager::Disconnect()
{
    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);

        if (!m_Connected)
        {
            return;
        }
    } // Release lock before calling DestroyPool()

    // DestroyPool doesn't need the lock - it's safe to call after disconnecting
    DestroyPool();

    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);
        m_Connected = false;
        m_DatabasePath.clear();
    }

    Log::Info("Disconnected from database");
}

bool DatabaseManager::IsConnected() const
{
    std::lock_guard<std::mutex> lock(m_PoolMutex);
    return m_Connected && !m_ConnectionPool.empty();
}

std::optional<DatabaseError> DatabaseManager::GetLastError() const
{
    std::lock_guard<std::mutex> lock(m_ErrorMutex);
    return m_LastError;
}

//=================================================================================================
// Transaction Management
//=================================================================================================

bool DatabaseManager::BeginTransaction()
{
    if (!m_Connected)
    {
        Log::Error("Cannot begin transaction: not connected");
        return false;
    }

    std::lock_guard<std::mutex> txnLock(m_TransactionMutex);

    if (!m_TransactionConnection.empty())
    {
        Log::Error("Transaction already active");
        return false;
    }

    // Acquire connection and KEEP it for the transaction lifetime
    m_TransactionConnection = AcquireConnection();
    if (m_TransactionConnection.empty())
    {
        Log::Error("Failed to acquire connection for transaction");
        return false;
    }

    QSqlDatabase db = GetDatabase(m_TransactionConnection);
    if (!db.transaction())
    {
        StoreLastError(m_TransactionConnection);
        ReleaseConnection(m_TransactionConnection);
        m_TransactionConnection.clear();
        Log::Error("Failed to begin transaction: {}", m_LastError->message);
        return false;
    }

    Log::Debug("Transaction began on connection {}", m_TransactionConnection);
    return true;
}

bool DatabaseManager::Commit()
{
    std::lock_guard<std::mutex> txnLock(m_TransactionMutex);

    if (m_TransactionConnection.empty())
    {
        Log::Error("No active transaction to commit");
        return false;
    }

    QSqlDatabase db = GetDatabase(m_TransactionConnection);
    if (!db.commit())
    {
        StoreLastError(m_TransactionConnection);
        Log::Error("Failed to commit transaction: {}", m_LastError->message);
        // Don't release connection yet - might need to rollback
        return false;
    }

    // Release the connection back to the pool
    ReleaseConnection(m_TransactionConnection);
    m_TransactionConnection.clear();

    Log::Debug("Transaction committed");
    return true;
}

bool DatabaseManager::Rollback()
{
    std::lock_guard<std::mutex> txnLock(m_TransactionMutex);

    if (m_TransactionConnection.empty())
    {
        Log::Error("No active transaction to rollback");
        return false;
    }

    QSqlDatabase db = GetDatabase(m_TransactionConnection);
    if (!db.rollback())
    {
        StoreLastError(m_TransactionConnection);
        Log::Error("Failed to rollback transaction: {}", m_LastError->message);
        // Still release the connection
    }

    ReleaseConnection(m_TransactionConnection);
    m_TransactionConnection.clear();

    Log::Debug("Transaction rolled back");
    return true;
}

//=================================================================================================
// Query Execution
//=================================================================================================

bool DatabaseManager::Execute(const std::string& sql)
{
    if (!m_Connected)
    {
        Log::Error("Cannot execute: not connected");
        return false;
    }

    // Check if we have an active transaction
    std::string connectionName;
    bool        useTransaction = false;
    {
        std::lock_guard<std::mutex> txnLock(m_TransactionMutex);
        if (!m_TransactionConnection.empty())
        {
            connectionName = m_TransactionConnection;
            useTransaction = true;
        }
    }

    if (useTransaction)
    {
        // Use transaction connection directly (no guard needed, transaction owns it)
        QSqlDatabase db = GetDatabase(connectionName);
        QSqlQuery    query(db);

        if (!query.exec(QString::fromStdString(sql)))
        {
            StoreQueryError(query);
            Log::Error("Failed to execute SQL: {}", m_LastError->message);
            Log::Debug("SQL: {}", sql);
            return false;
        }

        Log::Trace("Executed SQL: {}", sql);
        return true;
    }
    else
    {
        // Normal execution with pooled connection
        PooledConnectionGuard guard(*this, AcquireConnection());
        if (guard.GetConnectionName().empty())
        {
            Log::Error("Failed to acquire connection");
            return false;
        }

        QSqlDatabase db = GetDatabase(guard.GetConnectionName());
        QSqlQuery    query(db);

        if (!query.exec(QString::fromStdString(sql)))
        {
            StoreQueryError(query);
            Log::Error("Failed to execute SQL: {}", m_LastError->message);
            Log::Debug("SQL: {}", sql);
            return false;
        }

        Log::Trace("Executed SQL: {}", sql);
        return true;
    }
}

std::optional<QueryResult> DatabaseManager::Query(const std::string& sql)
{
    if (!m_Connected)
    {
        Log::Error("Cannot query: not connected");
        return std::nullopt;
    }

    // Check if we have an active transaction
    std::string connectionName;
    bool        useTransaction = false;
    {
        std::lock_guard<std::mutex> txnLock(m_TransactionMutex);
        if (!m_TransactionConnection.empty())
        {
            connectionName = m_TransactionConnection;
            useTransaction = true;
        }
    }

    if (useTransaction)
    {
        // Use transaction connection directly
        QSqlDatabase db = GetDatabase(connectionName);
        QSqlQuery    query(db);

        if (!query.exec(QString::fromStdString(sql)))
        {
            StoreQueryError(query);
            Log::Error("Failed to execute query: {}", m_LastError->message);
            Log::Debug("SQL: {}", sql);
            return std::nullopt;
        }

        auto result = ConvertQueryResult(query);
        Log::Trace("Query returned {} rows", result.GetRowCount());
        return result;
    }
    else
    {
        // Normal query with pooled connection
        PooledConnectionGuard guard(*this, AcquireConnection());
        if (guard.GetConnectionName().empty())
        {
            Log::Error("Failed to acquire connection");
            return std::nullopt;
        }

        QSqlDatabase db = GetDatabase(guard.GetConnectionName());
        QSqlQuery    query(db);

        if (!query.exec(QString::fromStdString(sql)))
        {
            StoreLastError(guard.GetConnectionName());
            Log::Error("Failed to execute query: {}", m_LastError->message);
            Log::Debug("SQL: {}", sql);
            return std::nullopt;
        }

        auto result = ConvertQueryResult(query);
        Log::Trace("Query returned {} rows", result.GetRowCount());
        return result;
    }
}

bool DatabaseManager::ExecutePrepared(const std::string&              sql,
                                      const std::vector<std::string>& params)
{
    if (!m_Connected)
    {
        Log::Error("Cannot execute prepared: not connected");
        return false;
    }

    // Check if we have an active transaction
    std::string connectionName;
    bool        useTransaction = false;
    {
        std::lock_guard<std::mutex> txnLock(m_TransactionMutex);
        if (!m_TransactionConnection.empty())
        {
            connectionName = m_TransactionConnection;
            useTransaction = true;
        }
    }

    if (useTransaction)
    {
        QSqlDatabase db = GetDatabase(connectionName);
        QSqlQuery    query(db);
        query.prepare(QString::fromStdString(sql));

        for (const auto& param : params)
        {
            query.addBindValue(QString::fromStdString(param));
        }

        if (!query.exec())
        {
            StoreQueryError(query);
            Log::Error("Failed to execute prepared statement: {}", m_LastError->message);
            Log::Debug("SQL: {}", sql);
            return false;
        }

        Log::Trace("Executed prepared statement");
        return true;
    }
    else
    {
        PooledConnectionGuard guard(*this, AcquireConnection());
        if (guard.GetConnectionName().empty())
        {
            Log::Error("Failed to acquire connection");
            return false;
        }

        QSqlDatabase db = GetDatabase(guard.GetConnectionName());
        QSqlQuery    query(db);
        query.prepare(QString::fromStdString(sql));

        for (const auto& param : params)
        {
            query.addBindValue(QString::fromStdString(param));
        }

        if (!query.exec())
        {
            StoreQueryError(query);
            Log::Error("Failed to execute prepared statement: {}", m_LastError->message);
            Log::Debug("SQL: {}", sql);
            return false;
        }

        Log::Trace("Executed prepared statement");
        return true;
    }
}

std::optional<QueryResult> DatabaseManager::QueryPrepared(const std::string&              sql,
                                                          const std::vector<std::string>& params)
{
    if (!m_Connected)
    {
        Log::Error("Cannot query prepared: not connected");
        return std::nullopt;
    }

    // Check if we have an active transaction
    std::string connectionName;
    bool        useTransaction = false;
    {
        std::lock_guard<std::mutex> txnLock(m_TransactionMutex);
        if (!m_TransactionConnection.empty())
        {
            connectionName = m_TransactionConnection;
            useTransaction = true;
        }
    }

    if (useTransaction)
    {
        QSqlDatabase db = GetDatabase(connectionName);
        QSqlQuery    query(db);
        query.prepare(QString::fromStdString(sql));

        for (const auto& param : params)
        {
            query.addBindValue(QString::fromStdString(param));
        }

        if (!query.exec())
        {
            StoreQueryError(query);
            Log::Error("Failed to execute prepared query: {}", m_LastError->message);
            Log::Debug("SQL: {}", sql);
            return std::nullopt;
        }

        auto result = ConvertQueryResult(query);
        Log::Trace("Prepared query returned {} rows", result.GetRowCount());
        return result;
    }
    else
    {
        PooledConnectionGuard guard(*this, AcquireConnection());
        if (guard.GetConnectionName().empty())
        {
            Log::Error("Failed to acquire connection");
            return std::nullopt;
        }

        QSqlDatabase db = GetDatabase(guard.GetConnectionName());
        QSqlQuery    query(db);
        query.prepare(QString::fromStdString(sql));

        for (const auto& param : params)
        {
            query.addBindValue(QString::fromStdString(param));
        }

        if (!query.exec())
        {
            StoreQueryError(query);
            Log::Error("Failed to execute prepared query: {}", m_LastError->message);
            Log::Debug("SQL: {}", sql);
            return std::nullopt;
        }

        auto result = ConvertQueryResult(query);
        Log::Trace("Prepared query returned {} rows", result.GetRowCount());
        return result;
    }
}

//=================================================================================================
// Schema Management
//=================================================================================================

bool DatabaseManager::InitializeSchema()
{
    // Use first connection for schema operations
    auto connectionName = AcquireConnection();
    if (connectionName.empty())
    {
        return false;
    }

    PooledConnectionGuard guard(*this, connectionName);
    QSqlDatabase          db = GetDatabase(connectionName);

    // Check if schema already exists
    QSqlQuery query(db);
    if (!query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='planets'"))
    {
        StoreLastError(connectionName);
        return false;
    }

    if (query.next())
    {
        Log::Info("Database schema already initialized");
        return true;
    }

    Log::Info("Initializing database schema...");

    // Create schema version table
    const std::string schemaVersionSql = R"(
        CREATE TABLE IF NOT EXISTS schema_version (
            version INTEGER PRIMARY KEY,
            applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";

    query.exec(QString::fromStdString(schemaVersionSql));

    // Create planets table
    const std::string planetsSql = R"(
        CREATE TABLE planets (
            planet_id TEXT PRIMARY KEY,
            seed INTEGER NOT NULL,
            name TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            tectonic_history BLOB,
            global_uplift_map BLOB,
            plate_configuration BLOB,
            resolution_km INTEGER,
            metadata TEXT
        )
    )";

    if (!query.exec(QString::fromStdString(planetsSql)))
    {
        StoreLastError(connectionName);
        return false;
    }

    // Create storage_regions table
    const std::string regionsSql = R"(
        CREATE TABLE storage_regions (
            region_id TEXT PRIMARY KEY,
            planet_id TEXT NOT NULL REFERENCES planets(planet_id),
            grid_x INTEGER NOT NULL,
            grid_y INTEGER NOT NULL,
            size_km INTEGER NOT NULL,
            resolution_m INTEGER NOT NULL,
            heightmap BLOB,
            river_network BLOB,
            geology_data BLOB,
            border_data BLOB,
            generated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(planet_id, grid_x, grid_y)
        )
    )";

    if (!query.exec(QString::fromStdString(regionsSql)))
    {
        StoreLastError(connectionName);
        return false;
    }

    // Create spatial index
    if (!query.exec("CREATE INDEX idx_region_lookup ON storage_regions(planet_id, grid_x, grid_y)"))
    {
        StoreLastError(connectionName);
        return false;
    }

    // Record schema version
    query.prepare("INSERT INTO schema_version (version) VALUES (?)");
    query.addBindValue(1);
    if (!query.exec())
    {
        StoreLastError(connectionName);
        return false;
    }

    Log::Info("Database schema initialized successfully (version 1)");
    return true;
}

int32_t DatabaseManager::GetSchemaVersion() const
{
    auto result =
        const_cast<DatabaseManager*>(this)->Query("SELECT MAX(version) FROM schema_version");

    if (!result || result->IsEmpty())
    {
        return 0;
    }

    try
    {
        return std::stoi(result->rows[0][0]);
    }
    catch (...)
    {
        return 0;
    }
}

bool DatabaseManager::MigrateSchema(int32_t targetVersion)
{
    int32_t currentVersion = GetSchemaVersion();

    if (currentVersion >= targetVersion)
    {
        Log::Info("Schema already at version {} (target: {})", currentVersion, targetVersion);
        return true;
    }

    Log::Info("Migrating schema from version {} to {}", currentVersion, targetVersion);

    // Future migrations will be added here
    return true;
}

//=================================================================================================
// Connection Pool Management
//=================================================================================================

void DatabaseManager::SetPoolSize(int32_t poolSize)
{
    std::lock_guard<std::mutex> lock(m_PoolMutex);

    if (m_Connected)
    {
        Log::Error("Cannot change pool size while connected");
        return;
    }

    if (poolSize < 1)
    {
        Log::Error("Pool size must be at least 1");
        return;
    }

    m_PoolSize = poolSize;
    Log::Info("Connection pool size set to {}", m_PoolSize);
}

int32_t DatabaseManager::GetPoolSize() const
{
    std::lock_guard<std::mutex> lock(m_PoolMutex);
    return m_PoolSize;
}

bool DatabaseManager::InitializePool()
{
    m_ConnectionPool.clear();
    m_ConnectionPool.reserve(static_cast<size_t>(m_PoolSize));

    for (int32_t i = 0; i < m_PoolSize; ++i)
    {
        std::string connectionName = "orogena_db_" + std::to_string(i);

        QSqlDatabase db =
            QSqlDatabase::addDatabase("QSQLITE", QString::fromStdString(connectionName));
        db.setDatabaseName(QString::fromStdString(m_DatabasePath));

        if (!db.open())
        {
            Log::Error("Failed to open pooled connection {}: {}", connectionName,
                       db.lastError().text().toStdString());
            DestroyPool();
            return false;
        }

        // Enable WAL mode for better concurrent access
        QSqlQuery pragmaQuery(db);
        pragmaQuery.exec("PRAGMA journal_mode=WAL");
        pragmaQuery.exec("PRAGMA busy_timeout=5000");

        m_ConnectionPool.push_back({connectionName, false});
        Log::Debug("Created pooled connection {}", connectionName);
    }

    Log::Info("Connection pool initialized with {} connections", m_PoolSize);
    return true;
}

void DatabaseManager::DestroyPool()
{
    for (const auto& conn : m_ConnectionPool)
    {
        QSqlDatabase::removeDatabase(QString::fromStdString(conn.connectionName));
        Log::Debug("Removed pooled connection {}", conn.connectionName);
    }

    m_ConnectionPool.clear();
    Log::Info("Connection pool destroyed");
}

std::string DatabaseManager::AcquireConnection(int32_t timeoutMs)
{
    std::unique_lock<std::mutex> lock(m_PoolMutex);

    // Wait for available connection
    if (timeoutMs > 0)
    {
        auto timeout = std::chrono::milliseconds(timeoutMs);
        if (!m_PoolCondition.wait_for(lock, timeout,
                                      [this]()
                                      {
                                          return std::any_of(m_ConnectionPool.begin(),
                                                             m_ConnectionPool.end(),
                                                             [](const PooledConnection& conn)
                                                             { return !conn.inUse; });
                                      }))
        {
            Log::Warn("Timeout waiting for database connection ({} ms)", timeoutMs);
            return "";
        }
    }
    else
    {
        m_PoolCondition.wait(lock,
                             [this]()
                             {
                                 return std::any_of(
                                     m_ConnectionPool.begin(), m_ConnectionPool.end(),
                                     [](const PooledConnection& conn) { return !conn.inUse; });
                             });
    }

    // Find and mark connection as in use
    for (auto& conn : m_ConnectionPool)
    {
        if (!conn.inUse)
        {
            conn.inUse = true;
            Log::Trace("Acquired connection {}", conn.connectionName);
            return conn.connectionName;
        }
    }

    // Should never reach here due to wait condition
    Log::Error("Failed to acquire connection despite wait condition");
    return "";
}

void DatabaseManager::ReleaseConnection(const std::string& connectionName)
{
    std::lock_guard<std::mutex> lock(m_PoolMutex);

    for (auto& conn : m_ConnectionPool)
    {
        if (conn.connectionName == connectionName)
        {
            conn.inUse = false;
            Log::Trace("Released connection {}", connectionName);
            m_PoolCondition.notify_one();
            return;
        }
    }

    Log::Warn("Attempted to release unknown connection {}", connectionName);
}

QSqlDatabase DatabaseManager::GetDatabase(const std::string& connectionName)
{
    return QSqlDatabase::database(QString::fromStdString(connectionName));
}

//=============================================================================================
// Private Functions
//=============================================================================================

QueryResult DatabaseManager::ConvertQueryResult(QSqlQuery& query)
{
    QueryResult result;

    QSqlRecord record = query.record();
    for (int32_t i = 0; i < record.count(); ++i)
    {
        result.columnNames.push_back(record.fieldName(i).toStdString());
    }

    while (query.next())
    {
        std::vector<std::string> row;
        for (int32_t i = 0; i < record.count(); ++i)
        {
            row.push_back(query.value(i).toString().toStdString());
        }
        result.rows.push_back(std::move(row));
    }

    result.rowsAffected = query.numRowsAffected();
    return result;
}

void DatabaseManager::StoreLastError(const std::string& connectionName)
{
    std::lock_guard<std::mutex> lock(m_ErrorMutex);

    QSqlDatabase db =
        connectionName.empty()
            ? GetDatabase(m_ConnectionPool.empty() ? "" : m_ConnectionPool[0].connectionName)
            : GetDatabase(connectionName);

    QSqlError error = db.lastError();

    // Only store if there's actually an error
    if (error.isValid() && error.type() != QSqlError::NoError)
    {
        m_LastError = DatabaseError{.message = error.text().toStdString(),
                                    .sqlState = error.nativeErrorCode().toStdString(),
                                    .nativeErrorCode = static_cast<int32_t>(error.type())};
    }
}

void DatabaseManager::StoreQueryError(QSqlQuery& query)
{
    std::lock_guard<std::mutex> lock(m_ErrorMutex);
    QSqlError                   error = query.lastError();
    m_LastError = DatabaseError{.message = error.text().toStdString(),
                                .sqlState = error.nativeErrorCode().toStdString(),
                                .nativeErrorCode = static_cast<int32_t>(error.type())};
}

bool DatabaseManager::ExecuteMigration(const std::string& sql)
{
    DatabaseTransaction txn(*this);

    if (!Execute(sql))
    {
        return false;
    }

    if (!txn.Commit())
    {
        return false;
    }

    return true;
}

} // namespace Orogena::Database
