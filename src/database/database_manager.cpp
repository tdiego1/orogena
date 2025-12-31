/**************************************************************************************************/
/**
 * @file database_manager.cpp
 * @brief Implementation of DatabaseManager
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
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
std::mutex DatabaseManager::s_InstanceMutex;

//=================================================================================================
// PooledConnectionGuard Implementation
//=================================================================================================

PooledConnectionGuard::PooledConnectionGuard(DatabaseManager& manager,
                                             const std::string& connectionName)
    : m_Manager(manager), m_ConnectionName(connectionName)
{
}

PooledConnectionGuard::~PooledConnectionGuard()
{
    m_Manager.ReleaseConnection(m_ConnectionName);
}

//=================================================================================================
// DatabaseTransaction Implementation
//=================================================================================================

DatabaseTransaction::DatabaseTransaction(IDatabase& db)
    : m_Database(db), m_Committed(false), m_RolledBack(false)
{
    if (!m_Database.BeginTransaction())
    {
        throw std::runtime_error("Failed to begin database transaction");
    }
}

DatabaseTransaction::~DatabaseTransaction()
{
    if (!m_Committed && !m_RolledBack)
    {
        m_Database.Rollback();
        Log::Warn("Transaction auto-rolled back (not explicitly committed)");
    }
}

bool DatabaseTransaction::Commit()
{
    if (m_Committed)
    {
        Log::Warn("Transaction already committed");
        return true;
    }

    if (m_RolledBack)
    {
        Log::Error("Cannot commit transaction after rollback");
        return false;
    }

    m_Committed = m_Database.Commit();
    return m_Committed;
}

bool DatabaseTransaction::Rollback()
{
    if (m_RolledBack)
    {
        Log::Warn("Transaction already rolled back");
        return true;
    }

    m_RolledBack = m_Database.Rollback();
    m_Committed = false;
    return m_RolledBack;
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
    std::lock_guard<std::mutex> lock(m_PoolMutex);

    if (m_Connected)
    {
        Log::Warn("Already connected to database, disconnecting first");
        Disconnect();
    }

    m_DatabasePath = path;

    if (!InitializePool())
    {
        Log::Error("Failed to initialize connection pool");
        return false;
    }

    m_Connected = true;
    Log::Info("Connected to database: {}", path);

    // Initialize schema using first connection
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
    std::lock_guard<std::mutex> lock(m_PoolMutex);

    if (!m_Connected)
    {
        return;
    }

    DestroyPool();
    m_Connected = false;
    m_DatabasePath.clear();
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

    // Transactions use the first connection (main thread)
    auto connectionName = AcquireConnection();
    if (connectionName.empty())
    {
        Log::Error("Failed to acquire connection for transaction");
        return false;
    }

    QSqlDatabase db = GetDatabase(connectionName);
    if (!db.transaction())
    {
        StoreLastError(connectionName);
        ReleaseConnection(connectionName);
        Log::Error("Failed to begin transaction: {}", m_LastError->message);
        return false;
    }

    Log::Debug("Transaction began on connection {}", connectionName);
    return true;
}

bool DatabaseManager::Commit()
{
    // Find connection with active transaction
    std::string activeConnection;
    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);
        for (const auto& conn : m_ConnectionPool)
        {
            if (conn.inUse)
            {
                activeConnection = conn.connectionName;
                break;
            }
        }
    }

    if (activeConnection.empty())
    {
        Log::Error("No active transaction to commit");
        return false;
    }

    QSqlDatabase db = GetDatabase(activeConnection);
    if (!db.commit())
    {
        StoreLastError(activeConnection);
        ReleaseConnection(activeConnection);
        Log::Error("Failed to commit transaction: {}", m_LastError->message);
        return false;
    }

    ReleaseConnection(activeConnection);
    Log::Debug("Transaction committed on connection {}", activeConnection);
    return true;
}

bool DatabaseManager::Rollback()
{
    // Find connection with active transaction
    std::string activeConnection;
    {
        std::lock_guard<std::mutex> lock(m_PoolMutex);
        for (const auto& conn : m_ConnectionPool)
        {
            if (conn.inUse)
            {
                activeConnection = conn.connectionName;
                break;
            }
        }
    }

    if (activeConnection.empty())
    {
        Log::Error("No active transaction to rollback");
        return false;
    }

    QSqlDatabase db = GetDatabase(activeConnection);
    if (!db.rollback())
    {
        StoreLastError(activeConnection);
        ReleaseConnection(activeConnection);
        Log::Error("Failed to rollback transaction: {}", m_LastError->message);
        return false;
    }

    ReleaseConnection(activeConnection);
    Log::Debug("Transaction rolled back on connection {}", activeConnection);
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

    PooledConnectionGuard guard(*this, AcquireConnection());
    if (guard.GetConnectionName().empty())
    {
        Log::Error("Failed to acquire connection");
        return false;
    }

    QSqlDatabase db = GetDatabase(guard.GetConnectionName());
    QSqlQuery query(db);

    if (!query.exec(QString::fromStdString(sql)))
    {
        StoreLastError(guard.GetConnectionName());
        Log::Error("Failed to execute SQL: {}", m_LastError->message);
        Log::Debug("SQL: {}", sql);
        return false;
    }

    Log::Trace("Executed SQL: {}", sql);
    return true;
}

std::optional<QueryResult> DatabaseManager::Query(const std::string& sql)
{
    if (!m_Connected)
    {
        Log::Error("Cannot query: not connected");
        return std::nullopt;
    }

    PooledConnectionGuard guard(*this, AcquireConnection());
    if (guard.GetConnectionName().empty())
    {
        Log::Error("Failed to acquire connection");
        return std::nullopt;
    }

    QSqlDatabase db = GetDatabase(guard.GetConnectionName());
    QSqlQuery query(db);

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

bool DatabaseManager::ExecutePrepared(const std::string& sql,
                                      const std::vector<std::string>& params)
{
    if (!m_Connected)
    {
        Log::Error("Cannot execute prepared: not connected");
        return false;
    }

    PooledConnectionGuard guard(*this, AcquireConnection());
    if (guard.GetConnectionName().empty())
    {
        Log::Error("Failed to acquire connection");
        return false;
    }

    QSqlDatabase db = GetDatabase(guard.GetConnectionName());
    QSqlQuery query(db);
    query.prepare(QString::fromStdString(sql));

    for (const auto& param : params)
    {
        query.addBindValue(QString::fromStdString(param));
    }

    if (!query.exec())
    {
        StoreLastError(guard.GetConnectionName());
        Log::Error("Failed to execute prepared statement: {}", m_LastError->message);
        Log::Debug("SQL: {}", sql);
        return false;
    }

    Log::Trace("Executed prepared statement");
    return true;
}

std::optional<QueryResult> DatabaseManager::QueryPrepared(const std::string& sql,
                                                          const std::vector<std::string>& params)
{
    if (!m_Connected)
    {
        Log::Error("Cannot query prepared: not connected");
        return std::nullopt;
    }

    PooledConnectionGuard guard(*this, AcquireConnection());
    if (guard.GetConnectionName().empty())
    {
        Log::Error("Failed to acquire connection");
        return std::nullopt;
    }

    QSqlDatabase db = GetDatabase(guard.GetConnectionName());
    QSqlQuery query(db);
    query.prepare(QString::fromStdString(sql));

    for (const auto& param : params)
    {
        query.addBindValue(QString::fromStdString(param));
    }

    if (!query.exec())
    {
        StoreLastError(guard.GetConnectionName());
        Log::Error("Failed to execute prepared query: {}", m_LastError->message);
        Log::Debug("SQL: {}", sql);
        return std::nullopt;
    }

    auto result = ConvertQueryResult(query);
    Log::Trace("Prepared query returned {} rows", result.GetRowCount());
    return result;
}

//=================================================================================================
// Schema Management
//=================================================================================================

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
    QSqlDatabase db = GetDatabase(connectionName);

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
