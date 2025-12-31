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

#include "database/database_interface.h"

#include <mutex>

// Forward declare Qt types to avoid leaking Qt headers
class QSqlDatabase;
class QSqlQuery;

namespace Orogena::Database
{

/**
 * @brief SQLite database manager with singleton access
 *
 * @details
 * Thread-safe singleton managing SQLite database connection.
 * Uses Qt SQL internally but exposes pure C++ interface.
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

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Convert QSqlQuery results to QueryResult
     * @param query Executed QSqlQuery
     * @return QueryResult Converted results
     */
    QueryResult ConvertQueryResult(QSqlQuery& query);

    /**
     * @brief Store last error from QSqlDatabase
     */
    void StoreLastError();

    /**
     * @brief Execute schema migration SQL
     * @param sql Migration SQL statement
     * @return true if successful, false otherwise
     */
    bool ExecuteMigration(const std::string& sql);

    //=============================================================================================
    // Private Members
    //=============================================================================================

    std::unique_ptr<QSqlDatabase> m_Database; ///< Qt SQL database connection.
    mutable std::mutex m_Mutex;               ///< Mutex for thread safety.
    std::optional<DatabaseError> m_LastError; ///< Last error information.
    bool m_Connected;                         ///< Connection status.

    static std::unique_ptr<DatabaseManager> s_Instance; ///< Singleton instance.
    static std::mutex s_InstanceMutex;                  ///< Mutex for singleton instance creation.
};

} // namespace Orogena::Database
