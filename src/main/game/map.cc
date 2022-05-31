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

#include "game/map.h"

#include <algorithm>
#include <execution>

#include "glm/gtc/constants.hpp"

using namespace std;
using namespace glm;

namespace airewar::game {
namespace {
/**
 * convert lat-lon-radius to cartesian coordinates
 *
 * @param lat latitude, in radians north
 * @param lon longitude, in radians east
 */
constexpr vec3 sphericalToCartesian(float lat, float lon) {
  return Map::RADIUS * vec3{sin(lon) * cos(lat), sin(lat), cos(lon) * cos(lat)};
}
}  // namespace

void Map::initTriangles() noexcept { root_ = make_unique<IcosaNode>(); }

Map::IcosaNode::IcosaNode() noexcept : children_() {
  // north polar cap
  for (size_t idx : {0, 1, 2, 3, 4})
    children_[idx] = make_unique<TriangleNode>(array<vec3, 3>{
        sphericalToCartesian(pi<float>() / 2.0f, 0.0f),
        sphericalToCartesian(atan(0.5f), idx * pi<float>() / 5.0f),
        sphericalToCartesian(atan(0.5f), (idx + 1) * pi<float>() / 5.0f)});

  // south polar cap
  for (size_t idx : {0, 1, 2, 3, 4})
    children_[idx + 5] = make_unique<TriangleNode>(array<vec3, 3>{
        sphericalToCartesian(-pi<float>() / 2.0f, 0.0f),
        sphericalToCartesian(-atan(0.5f), (idx + 1.5f) * pi<float>() / 5.0f),
        sphericalToCartesian(-atan(0.5f), (idx + 0.5f) * pi<float>() / 5.0f)});

  // pointy north half of equator
  for (size_t idx : {0, 1, 2, 3, 4})
    children_[idx + 10] = make_unique<TriangleNode>(array<vec3, 3>{
        sphericalToCartesian(atan(0.5f), idx * pi<float>() / 5.0f),
        sphericalToCartesian(-atan(0.5f), (idx - 0.5f) * pi<float>() / 5.0f),
        sphericalToCartesian(-atan(0.5f), (idx + 0.5f) * pi<float>() / 5.0f)});

  // flat north half of equator
  for (size_t idx : {0, 1, 2, 3, 4})
    children_[idx + 15] = make_unique<TriangleNode>(array<vec3, 3>{
        sphericalToCartesian(atan(0.5f), idx * pi<float>() / 5.0f),
        sphericalToCartesian(-atan(0.5f), (idx + 0.5f) * pi<float>() / 5.0f),
        sphericalToCartesian(atan(0.5f), (idx + 1.0f) * pi<float>() / 5.0f)});

  projectOntoSphere();
}

void Map::IcosaNode::projectOntoSphere() noexcept {
  for (auto &child : children_) child->projectOntoSphere();
}

Map::TriangularNode::TriangularNode(array<vec3, 3> const &vertices) noexcept
    : vertices_(vertices) {}

void Map::TriangularNode::projectOntoSphere() noexcept {
  for (auto &vertex : vertices_) vertex = RADIUS * normalize(vertex);
}

Map::TriangleNode::TriangleNode(array<vec3, 3> const &vertices) noexcept
    : TriangularNode(vertices), children_() {
  vec3 half0 = (vertices_[1] + vertices_[2]) / 2.0f;
  vec3 half1 = (vertices_[0] + vertices_[2]) / 2.0f;
  vec3 half2 = (vertices_[0] + vertices_[1]) / 2.0f;
  if (distance(vertices_[0], vertices_[1]) <= Map::MAX_TILE_SIZE) {
    children_[0] =
        make_unique<LeafNode>(array<vec3, 3>{vertices_[0], half2, half1});
    children_[1] =
        make_unique<LeafNode>(array<vec3, 3>{vertices_[1], half0, half2});
    children_[2] =
        make_unique<LeafNode>(array<vec3, 3>{vertices_[2], half1, half0});
    children_[3] = make_unique<LeafNode>(array<vec3, 3>{half0, half1, half2});
  } else {
    children_[0] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices_[0], half2, half1});
    children_[1] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices_[1], half0, half2});
    children_[2] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices_[2], half1, half0});
    children_[3] =
        make_unique<TriangleNode>(array<vec3, 3>{half0, half1, half2});
  }
}

void Map::TriangleNode::projectOntoSphere() noexcept {
  TriangularNode::projectOntoSphere();
  for (auto &child : children_) child->projectOntoSphere();
}

Map::LeafNode::LeafNode(array<vec3, 3> const &vertices) noexcept
    : TriangularNode(vertices), tile_() {}
}  // namespace airewar::game
