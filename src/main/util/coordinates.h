// Copyright 2022 Justin Hu
//
// This file is part of AireWar.
//
// AireWar is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// AireWar is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with AireWar. If not, see <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef AIREWAR_UTIL_COORDINATES_H_
#define AIREWAR_UTIL_COORDINATES_H_

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

namespace airewar::util {
/**
 * convert lat-lon-radius to cartesian coordinates
 *
 * @param lat latitude, in radians north
 * @param lon longitude, in radians east
 * @param radius radius of the planet
 */
constexpr glm::vec3 sphericalToCartesian(float lat, float lon, float radius) {
  return radius * glm::vec3{sin(lon) * cos(lat), sin(lat), cos(lon) * cos(lat)};
}

/**
 * convert cartesian to lat-lon-radius
 *
 * @param v coordinate to convert
 * @return output coordinates as lat-lon-radius
 */
constexpr glm::vec3 cartesianToSpherical(glm::vec3 const &v) {
  float radius = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  return glm::vec3{asin(v.y / radius),
                   atan2(v.x, v.z) < 0.0f
                       ? atan2(v.x, v.z) + glm::two_pi<float>()
                       : atan2(v.x, v.z),
                   radius};
}
}  // namespace airewar::util

#endif  // AIREWAR_UTIL_COORDINATES_H_
