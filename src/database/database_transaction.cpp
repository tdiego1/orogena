/**************************************************************************************************/
/**
 * @file database_transaction.cpp
 * @brief Implementation of DatabaseTransaction
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
