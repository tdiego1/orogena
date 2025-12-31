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
    : m_Database(std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"))),
      m_Connected(false)
{
    Log::Info("DatabaseManager initialized");
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
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (m_Connected)
    {
        Log::Warn("Already connected to database, disconnecting first");
        Disconnect();
    }

    m_Database->setDatabaseName(QString::fromStdString(path));

    if (!m_Database->open())
    {
        StoreLastError();
        Log::Error("Failed to open database: {}", m_LastError->message);
        return false;
    }

    m_Connected = true;
    Log::Info("Connected to database: {}", path);

    // Initialize schema if needed
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
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (!m_Connected)
    {
        return;
    }

    m_Database->close();
    m_Connected = false;
    Log::Info("Disconnected from database");
}

bool DatabaseManager::IsConnected() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Connected && m_Database->isOpen();
}

std::optional<DatabaseError> DatabaseManager::GetLastError() const
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_LastError;
}

//=================================================================================================
// Transaction Management
//=================================================================================================

bool DatabaseManager::BeginTransaction()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (!m_Connected)
    {
        Log::Error("Cannot begin transaction: not connected");
        return false;
    }

    if (!m_Database->transaction())
    {
        StoreLastError();
        Log::Error("Failed to begin transaction: {}", m_LastError->message);
        return false;
    }

    Log::Debug("Transaction began");
    return true;
}

bool DatabaseManager::Commit()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (!m_Connected)
    {
        Log::Error("Cannot commit transaction: not connected");
        return false;
    }

    if (!m_Database->commit())
    {
        StoreLastError();
        Log::Error("Failed to commit transaction: {}", m_LastError->message);
        return false;
    }

    Log::Debug("Transaction committed");
    return true;
}

bool DatabaseManager::Rollback()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (!m_Connected)
    {
        Log::Error("Cannot rollback transaction: not connected");
        return false;
    }

    if (!m_Database->rollback())
    {
        StoreLastError();
        Log::Error("Failed to rollback transaction: {}", m_LastError->message);
        return false;
    }

    Log::Debug("Transaction rolled back");
    return true;
}

//=================================================================================================
// Query Execution
//=================================================================================================

bool DatabaseManager::Execute(const std::string& sql)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (!m_Connected)
    {
        Log::Error("Cannot execute: not connected");
        return false;
    }

    QSqlQuery query(*m_Database);

    if (!query.exec(QString::fromStdString(sql)))
    {
        StoreLastError();
        Log::Error("Failed to execute SQL: {}", m_LastError->message);
        Log::Debug("SQL: {}", sql);
        return false;
    }

    Log::Trace("Executed SQL: {}", sql);
    return true;
}

std::optional<QueryResult> DatabaseManager::Query(const std::string& sql)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (!m_Connected)
    {
        Log::Error("Cannot query: not connected");
        return std::nullopt;
    }

    QSqlQuery query(*m_Database);

    if (!query.exec(QString::fromStdString(sql)))
    {
        StoreLastError();
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
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (!m_Connected)
    {
        Log::Error("Cannot execute prepared: not connected");
        return false;
    }

    QSqlQuery query(*m_Database);
    query.prepare(QString::fromStdString(sql));

    // Bind parameters
    for (const auto& param : params)
    {
        query.addBindValue(QString::fromStdString(param));
    }

    if (!query.exec())
    {
        StoreLastError();
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
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (!m_Connected)
    {
        Log::Error("Cannot query prepared: not connected");
        return std::nullopt;
    }

    QSqlQuery query(*m_Database);
    query.prepare(QString::fromStdString(sql));

    // Bind parameters
    for (const auto& param : params)
    {
        query.addBindValue(QString::fromStdString(param));
    }

    if (!query.exec())
    {
        StoreLastError();
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

bool DatabaseManager::InitializeSchema()
{
    // Check if schema already exists
    auto result = Query("SELECT name FROM sqlite_master WHERE type='table' AND name='planets'");
    if (result && !result->IsEmpty())
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

    if (!Execute(schemaVersionSql))
    {
        return false;
    }

    // Create planets table (Schema from SDP)
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

    if (!Execute(planetsSql))
    {
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

    if (!Execute(regionsSql))
    {
        return false;
    }

    // Create spatial index for regions
    const std::string indexSql =
        "CREATE INDEX idx_region_lookup ON storage_regions(planet_id, grid_x, grid_y)";

    if (!Execute(indexSql))
    {
        return false;
    }

    // Record schema version
    if (!ExecutePrepared("INSERT INTO schema_version (version) VALUES (?)", {"1"}))
    {
        return false;
    }

    Log::Info("Database schema initialized successfully (version 1)");
    return true;
}

int32_t DatabaseManager::GetSchemaVersion() const
{
    // This is acceptable because we're not actually modifying logical state
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
    // Example:
    // if (currentVersion == 1 && targetVersion >= 2) {
    //     if (!ExecuteMigration("ALTER TABLE planets ADD COLUMN new_field TEXT")) {
    //         return false;
    //     }
    //     currentVersion = 2;
    // }

    return true;
}

//=============================================================================================
// Private Functions
//=============================================================================================

QueryResult DatabaseManager::ConvertQueryResult(QSqlQuery& query)
{
    QueryResult result;

    // Get column names
    QSqlRecord record = query.record();
    for (int32_t i = 0; i < record.count(); ++i)
    {
        result.columnNames.push_back(record.fieldName(i).toStdString());
    }

    // Get rows
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

void DatabaseManager::StoreLastError()
{
    QSqlError error = m_Database->lastError();

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
