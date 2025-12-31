/**************************************************************************************************/
/**
 * @file database_transaction.cpp
 * @brief Implementation of DatabaseTransaction
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include "database/database_interface.h"
#include "utils/utils_logger.h"

#include <stdexcept>

namespace Orogena::Database
{
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

} // namespace Orogena::Database
