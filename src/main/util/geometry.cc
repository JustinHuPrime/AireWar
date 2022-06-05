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

#include "util/geometry.h"

#include <algorithm>

using namespace glm;
using namespace std;

namespace airewar::util {
vec3 sphericalToCartesian(float lat, float lon, float radius) noexcept {
  return radius * vec3{sin(lon) * cos(lat), sin(lat), cos(lon) * cos(lat)};
}

vec3 cartesianToSpherical(vec3 const &v) noexcept {
  float radius = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  return vec3{asin(v.y / radius),
              atan2(v.x, v.z) < 0.0f ? atan2(v.x, v.z) + glm::two_pi<float>()
                                     : atan2(v.x, v.z),
              radius};
}

bool rayIntersectsTriangle(vec3 const &ray,
                           array<vec3, 3> const &vertices) noexcept {
  vec3 direction = normalize(ray);
  vec3 edge1 = vertices[1] - vertices[0];
  vec3 edge2 = vertices[2] - vertices[0];
  vec3 normal = cross(edge1, edge2);
  float determinant = -dot(direction, normal);
  vec3 dao = cross(-vertices[0], direction);
  float u = dot(edge2, dao) / determinant;
  float v = -dot(edge1, dao) / determinant;
  float t = dot(-vertices[0], normal) / determinant;
  return abs(determinant) > INTERSECT_EPSILON && t >= 0.0 && u >= 0.0 &&
         v >= 0.0 && (u + v) <= 1.0;
}
}  // namespace airewar::util
