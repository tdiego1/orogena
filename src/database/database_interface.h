/**************************************************************************************************/
/**
 * @file database_interface.h
 * @brief Pure C++ interface for database operations (Qt-agnostic)
 *
 * @details
 * Defines abstract interfaces for database access following the Qt/Framework Boundary Separation
 * pattern. Implementations use Qt SQL internally but expose only std::string and standard C++
 * types.
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
#include <string>
#include <vector>

#include <optional>
#include <utils/utils_types.h>

namespace Orogena::Database
{

//=================================================================================================
// Query Result Types
//=================================================================================================

/**
 * @brief Result of a database query operation
 */
struct QueryResult
{
    std::vector<std::vector<std::string>> rows; ///< Rows of result data.
    std::vector<std::string> columnNames;       ///< Names of columns in result.
    int32_t rowsAffected{0};                    ///< Number of rows affected (for non-SELECT).

    /**
     * @brief Check if the result is empty
     *
     * @return true Result has no rows.
     * @return false Result has rows.
     */
    bool IsEmpty() const
    {
        return rows.empty();
    }

    /**
     * @brief Get number of rows in result
     *
     * @return size_t Number of rows.
     */
    size_t GetRowCount() const
    {
        return rows.size();
    }

    /**
     * @brief Get number of columns in result
     *
     * @return size_t Number of columns.
     */
    size_t GetColumnCount() const
    {
        return columnNames.empty() ? 0 : columnNames.size();
    }
};

/**
 * @brief Error information for database operations
 */
struct DatabaseError
{
    std::string message;        ///< Error message.
    std::string sqlState;       ///< SQL state code.
    int32_t nativeErrorCode{0}; ///< Native database error code.
};

//=================================================================================================
// Database Interface
//=================================================================================================

/**
 * @brief Abstract interface for database operations
 *
 * @details
 * Pure virtual interface for database access. Implementations must handle:
 * - Connection lifecycle
 * - Transaction management
 * - Query execution with prepared statements
 * - Error reporting
 *
 * All operations use standard C++ types (std::string, std::vector, std::optional).
 * Qt types are confined to the implementation layer.
 */
class IDatabase
{
  public:
    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    virtual ~IDatabase() = default;

    //=============================================================================================
    // Connection Management
    //=============================================================================================

    /**
     * @brief Connect to database at specified path
     *
     * @param path File path to SQLite database
     *
     * @return true if connection successful, false otherwise
     */
    virtual bool Connect(const std::string& path) = 0;

    /**
     * @brief Disconnect from database
     */
    virtual void Disconnect() = 0;

    /**
     * @brief Check if currently connected
     */
    virtual bool IsConnected() const = 0;

    /**
     * @brief Get last error information
     *
     * @return std::optional<DatabaseError> Last error if any occurred
     */
    virtual std::optional<DatabaseError> GetLastError() const = 0;

    //=============================================================================================
    // Transaction Management
    //=============================================================================================

    /**
     * @brief Begin a transaction
     * @return true if successful
     */
    virtual bool BeginTransaction() = 0;

    /**
     * @brief Commit current transaction
     * @return true if successful
     */
    virtual bool Commit() = 0;

    /**
     * @brief Rollback current transaction
     * @return true if successful
     */
    virtual bool Rollback() = 0;

    //=============================================================================================
    // Query Execution
    //=============================================================================================

    /**
     * @brief Execute SQL statement without returning results (INSERT, UPDATE, DELETE)
     * @param sql SQL statement
     * @return true if successful
     */
    virtual bool Execute(const std::string& sql) = 0;

    /**
     * @brief Execute SQL query and return results (SELECT)
     * @param sql SQL query
     * @return Query results or nullopt on error
     */
    virtual std::optional<QueryResult> Query(const std::string& sql) = 0;

    /**
     * @brief Execute prepared statement with parameters
     * @param sql SQL statement with placeholders (?)
     * @param params Parameter values (converted to strings)
     * @return true if successful
     */
    virtual bool ExecutePrepared(const std::string& sql,
                                 const std::vector<std::string>& params) = 0;

    /**
     * @brief Execute prepared query with parameters
     * @param sql SQL query with placeholders (?)
     * @param params Parameter values (converted to strings)
     * @return Query results or nullopt on error
     */
    virtual std::optional<QueryResult> QueryPrepared(const std::string& sql,
                                                     const std::vector<std::string>& params) = 0;
};

//=================================================================================================
// Transaction RAII Wrapper
//=================================================================================================

/**
 * @brief RAII wrapper for database transactions
 *
 * @details
 * Automatically begins transaction on construction and rolls back on destruction
 * unless explicitly committed. Ensures transaction safety even when exceptions occur.
 *
 * Example usage:
 * @code
 * {
 *     DatabaseTransaction txn(database);
 *     database.Execute("INSERT ...");
 *     database.Execute("UPDATE ...");
 *     txn.Commit(); // Commit if all operations succeeded
 * } // Auto-rollback if Commit() not called
 * @endcode
 */
class DatabaseTransaction
{
  public:
    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Begin transaction
     * @param db Database interface
     * @throws std::runtime_error if transaction cannot be started
     */
    explicit DatabaseTransaction(IDatabase& db);

    /**
     * @brief Rollback if not committed
     */
    ~DatabaseTransaction();

    // Delete copy, allow move
    DatabaseTransaction(const DatabaseTransaction&) = delete;
    DatabaseTransaction& operator=(const DatabaseTransaction&) = delete;
    DatabaseTransaction(DatabaseTransaction&&) = default;
    DatabaseTransaction& operator=(DatabaseTransaction&&) = delete;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Commit transaction
     * @return true if successful
     */
    bool Commit();

    /**
     * @brief Rollback transaction (explicit)
     * @return true if successful
     */
    bool Rollback();

  private:
    //=============================================================================================
    // Private Members
    //=============================================================================================

    IDatabase& m_Database; ///< Database interface reference.
    bool m_Committed;      ///< Whether transaction was committed.
    bool m_RolledBack;     ///< Whether transaction was rolled back.
};

} // namespace Orogena::Database
