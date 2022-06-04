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
namespace {
float signedVolume(vec3 const &a, vec3 const &b, vec3 const &c,
                   vec3 const &d) noexcept {
  return dot(cross(b - a, c - a), d - a);
}
}  // namespace

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
  // vec3 normalized = normalize(ray);
  // vec3 normal = cross(vertices_[1] - vertices_[0], vertices_[2] -
  // vertices_[0]); float a = dot(vertices_[0], normal); float b =
  // dot(normalized, normal); float k = a / b; vec3 intersect = normalized *
  // k; float barySum = (area(vertices_[0], vertices_[1], intersect) +
  //                  area(vertices_[1], vertices_[2], intersect) +
  //                  area(vertices_[2], vertices_[0], intersect)) /
  //                 area(vertices_[0], vertices_[1], vertices_[2]);
  // return 1.0f - INTERSECT_DELTA < barySum && barySum < 1.0f +
  // INTERSECT_DELTA;

  vec3 q1 = vec3{0.0f, 0.0f, 0.0f};
  vec3 q2 = normalize(ray) *
            length(*max_element(vertices.begin(), vertices.end(),
                                [](vec3 const &a, vec3 const &b) {
                                  return length(a) < length(b);
                                })) *
            10.0f;
  return signbit(signedVolume(q1, vertices[0], vertices[1], vertices[2])) !=
             signbit(signedVolume(q2, vertices[0], vertices[1], vertices[2])) &&
         signbit(signedVolume(q1, q2, vertices[0], vertices[1])) ==
             signbit(signedVolume(q1, q2, vertices[1], vertices[2])) &&
         signbit(signedVolume(q1, q2, vertices[1], vertices[2])) ==
             signbit(signedVolume(q1, q2, vertices[2], vertices[0]));
}
}  // namespace airewar::util
