/**************************************************************************************************/
/**
 * @file database_manager.h
 * @brief SQLite database manager implementation using Qt SQL
 *
 * @details
 * Concrete implementation of IDatabase using Qt SQL. All Qt types are confined to this
 * implementation file - the public interface uses only standard C++ types.
 *
 * Features:
 * - Singleton pattern for global access
 * - Connection pooling (Qt manages internally)
 * - Prepared statement support
 * - Transaction management with RAII
 * - Schema versioning and migration
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
/**************************************************************************************************/

#pragma once

#include <memory>

#include <queue>

#include "database/database_interface.h"

#include <condition_variable>
#include <mutex>

// Forward declare Qt types to avoid leaking Qt headers
class QSqlDatabase;
class QSqlQuery;

namespace Orogena::Database
{

/**
 * @brief Connection pool entry
 */
struct PooledConnection
{
    std::string connectionName; ///< Unique connection name.
    bool inUse{false};          ///< Connection usage status.
};

/**
 * @brief RAII wrapper for pooled database connection
 */
class PooledConnectionGuard
{
  public:
    PooledConnectionGuard(class DatabaseManager& manager, const std::string& connectionName);
    ~PooledConnectionGuard();

    PooledConnectionGuard(const PooledConnectionGuard&) = delete;
    PooledConnectionGuard& operator=(const PooledConnectionGuard&) = delete;

    const std::string& GetConnectionName() const
    {
        return m_ConnectionName;
    }

  private:
    DatabaseManager& m_Manager;
    std::string m_ConnectionName;
};

/**
 * @brief SQLite database manager with singleton access
 *
 * @details
 * Thread-safe singleton managing SQLite database connection.
 * Uses Qt SQL internally but exposes pure C++ interface.
 *
 * Connection pool allows concurrent database access from multiple threads.
 * Each thread aquires a connection from the pool, uses it, then returns it.
 */
class DatabaseManager : public IDatabase
{
  public:
    //=============================================================================================
    // Singleton Access
    //=============================================================================================

    /**
     * @brief Get singleton instance of DatabaseManager
     *
     * @return DatabaseManager& Reference to singleton instance.
     */
    static DatabaseManager& Instance();

    /**
     * @brief Destroy singleton instance
     */
    static void DestroyInstance();

    //=============================================================================================
    // IDatabase Implementation
    //=============================================================================================

    bool Connect(const std::string& path) override;
    void Disconnect() override;
    bool IsConnected() const override;
    std::optional<DatabaseError> GetLastError() const override;

    bool BeginTransaction() override;
    bool Commit() override;
    bool Rollback() override;

    bool Execute(const std::string& sql) override;
    std::optional<QueryResult> Query(const std::string& sql) override;
    bool ExecutePrepared(const std::string& sql, const std::vector<std::string>& params) override;
    std::optional<QueryResult> QueryPrepared(const std::string& sql,
                                             const std::vector<std::string>& params) override;

    //=============================================================================================
    // Schema Management
    //=============================================================================================

    /**
     * @brief Initialize database schema (creates tables if not exist)
     * @return true if successful
     */
    bool InitializeSchema();

    /**
     * @brief Get current schema version
     * @return Version number or 0 if not initialized
     */
    int32_t GetSchemaVersion() const;

    /**
     * @brief Migrate schema to target version
     * @param targetVersion Target schema version
     * @return true if successful
     */
    bool MigrateSchema(int32_t targetVersion);

    //=============================================================================================
    // Connection Pool Management
    //=============================================================================================

    /**
     * @brief Configure connection pool size
     * @param poolSize Number of connections in pool (default: 5)
     */
    void SetPoolSize(int32_t poolSize);

    /**
     * @brief Get current pool size
     * @return Number of connections in pool
     */
    int32_t GetPoolSize() const;

  private:
    //=============================================================================================
    // Private Constructor (Singleton)
    //=============================================================================================

    DatabaseManager();
    ~DatabaseManager();

    // Delete copy/move
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) = delete;
    DatabaseManager& operator=(DatabaseManager&&) = delete;

    // Allow unique_ptr to delete this object.
    friend std::default_delete<DatabaseManager>;
    friend class PooledConnectionGuard;

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Acquire connection from pool (blocks if none available)
     * @param timeoutMs Timeout in milliseconds (0 = wait forever)
     * @return Connection name or empty string on timeout
     */
    std::string AcquireConnection(int32_t timeoutMs = 0);

    /**
     * @brief Release connection back to pool
     * @param connectionName Connection to release
     */
    void ReleaseConnection(const std::string& connectionName);

    /**
     * @brief Initialize connection pool
     * @return true if successful
     */
    bool InitializePool();

    /**
     * @brief Destroy connection pool
     */
    void DestroyPool();

    /**
     * @brief Get QSqlDatabase for connection name
     * @param connectionName Connection name
     * @return QSqlDatabase reference
     */
    QSqlDatabase GetDatabase(const std::string& connectionName);

    /**
     * @brief Convert QSqlQuery results to QueryResult
     * @param query Executed QSqlQuery
     * @return QueryResult Converted results
     */
    QueryResult ConvertQueryResult(QSqlQuery& query);

    /**
     * @brief Store last error from QSqlDatabase
     */
    void StoreLastError(const std::string& connectionName = "");

    /**
     * @brief Execute schema migration SQL
     * @param sql Migration SQL statement
     * @return true if successful, false otherwise
     */
    bool ExecuteMigration(const std::string& sql);

    //=============================================================================================
    // Private Members
    //=============================================================================================

    std::string m_DatabasePath;                     ///< Path to database file.
    std::vector<PooledConnection> m_ConnectionPool; ///< Connection pool.
    mutable std::mutex m_PoolMutex;                 ///< Mutex for connection pool.
    std::condition_variable m_PoolCondition;        ///< Condition variable for pool.
    int32_t m_PoolSize;                             ///< Number of connections in pool.
    mutable std::mutex m_ErrorMutex;                ///< Mutex for error access;
    std::optional<DatabaseError> m_LastError;       ///< Last error information.
    bool m_Connected;                               ///< Connection status.

    static std::unique_ptr<DatabaseManager> s_Instance; ///< Singleton instance.
    static std::mutex s_InstanceMutex;                  ///< Mutex for singleton instance creation.
};

} // namespace Orogena::Database
