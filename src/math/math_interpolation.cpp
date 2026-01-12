/**************************************************************************************************/
/**
 * @file math_interpolation.cpp
 * @brief Implementation of interpolation functions
 *
 * @author Diego Torres
 * @date 2026
 * @copyright Copyright (C) 2026 Diego Torres
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

#include "math_interpolation.h"

#include <stdexcept>

namespace Orogena::Math
{
//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
float64_t PiecewiseLinear(float64_t x, const std::vector<std::pair<float64_t, float64_t>>& points)
{
    if (points.empty())
    {
        throw std::invalid_argument("PiecewiseLinear: points vector is empty");
    }

    if (points.size() == 1)
    {
        return points[0].second;
    }

    // Clamp to endpoints if outside range
    if (x <= points.front().first)
    {
        return points.front().second;
    }
    if (x >= points.back().first)
    {
        return points.back().second;
    }

    // Find the two adjacent points that bracket x
    for (size_t i = 0; i < points.size() - 1; ++i)
    {
        if (x >= points[i].first && x <= points[i + 1].first)
        {
            float64_t x0 = points[i].first;
            float64_t y0 = points[i].second;
            float64_t x1 = points[i + 1].first;
            float64_t y1 = points[i + 1].second;

            float64_t t = InverseLerp(x, x0, x1);
            return Lerp(t, y0, y1);
        }
    }

    // Should never reach here if points are sorted
    return points.back().second;
}

} // namespace Orogena::Math
