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
#include "util/coordinates.h"

using namespace std;
using namespace glm;
using namespace airewar::util;

namespace airewar::game {

void Map::generate(uint64_t seed) noexcept {
  seed_ = seed;
  root_ = make_unique<IcosaNode>();

  // step 1: generate plates and heightmap (see
  // https://www.youtube.com/watch?v=x_Tn66PvTn4)

  // step 1.1: generate plate boundaries

  // step 1.1.1: generate major plates

  // TODO

  // step 1.1.2: generate minor plates

  // TODO

  // NOTE: maybe use a voronoi diagram thing?

  // step 1.2: chose continental and oceanic plates

  // TODO

  // step 1.3: set plate motion (transform and rotational)

  // TODO

  // step 1.4: generate heightmap

  // step 1.4.1: continents + coastal islands

  // TODO

  // step 1.4.2: plate boundary islands

  // TODO

  // step 1.4.3: hotspot island arcs

  // TODO

  // step 2: calculate prevailing winds (see
  // https://www.youtube.com/watch?v=LifRswfCxFU)

  // step 2.1: generate trade winds

  // TODO

  // step 2.2: generate polar cells

  // TODO

  // step 3.3: generate ferrel cell

  // TODO

  // step 3: calculate currents (see
  // https://www.youtube.com/watch?v=n_E9UShtyY8)

  // step 3.1: equatorial gyres

  // TODO

  // step 3.2: ferrel gyres

  // TODO

  // step 3.3: circumpolar currents

  // TODO

  // step 3.4: fill in gaps

  // TODO

  // step 3.5: ENSO event zones

  // TODO

  // step 4: calculate biomes and climate (see
  // https://www.youtube.com/watch?v=5lCbxMZJ4zA and
  // https://www.youtube.com/watch?v=fag48Nh8PXE)

  // step 4.1: preperatory calculations

  // step 4.1.1: precipitation levels

  // TODO

  // step 4.1.2: temperature levels

  // TODO

  // step 4.1.3: orthographic lift

  // TODO

  // step 4.2: place biomes

  // step 4.2.1: mountain climates

  // TODO

  // step 4.2.2: tropical climates

  // TODO

  // step 4.2.3: continental climates

  // TODO
}

uint64_t Map::getSeed() const noexcept { return seed_; }

Map::IcosaNode::IcosaNode() noexcept : children_() {
  // NOTE: assumes at least one level of triangle nodes before the leaf nodes

  // north polar cap
  for (size_t idx : {0, 1, 2, 3, 4})
    children_[idx] = make_unique<TriangleNode>(array<vec3, 3>{
        sphericalToCartesian(half_pi<float>(), 0.0f, Map::RADIUS),
        sphericalToCartesian(atan(0.5f), idx * two_pi<float>() / 5.0f,
                             Map::RADIUS),
        sphericalToCartesian(atan(0.5f), (idx + 1) * two_pi<float>() / 5.0f,
                             Map::RADIUS)});

  // south polar cap
  for (size_t idx : {0, 1, 2, 3, 4})
    children_[idx + 5] = make_unique<TriangleNode>(array<vec3, 3>{
        sphericalToCartesian(-half_pi<float>(), 0.0f, Map::RADIUS),
        sphericalToCartesian(-atan(0.5f), (idx + 1.5f) * two_pi<float>() / 5.0f,
                             Map::RADIUS),
        sphericalToCartesian(-atan(0.5f), (idx + 0.5f) * two_pi<float>() / 5.0f,
                             Map::RADIUS)});

  // pointy north half of equator
  for (size_t idx : {0, 1, 2, 3, 4})
    children_[idx + 10] = make_unique<TriangleNode>(array<vec3, 3>{
        sphericalToCartesian(atan(0.5f), idx * two_pi<float>() / 5.0f,
                             Map::RADIUS),
        sphericalToCartesian(-atan(0.5f), (idx - 0.5f) * two_pi<float>() / 5.0f,
                             Map::RADIUS),
        sphericalToCartesian(-atan(0.5f), (idx + 0.5f) * two_pi<float>() / 5.0f,
                             Map::RADIUS)});

  // flat north half of equator
  for (size_t idx : {0, 1, 2, 3, 4})
    children_[idx + 15] = make_unique<TriangleNode>(array<vec3, 3>{
        sphericalToCartesian(atan(0.5f), idx * two_pi<float>() / 5.0f,
                             Map::RADIUS),
        sphericalToCartesian(-atan(0.5f), (idx + 0.5f) * two_pi<float>() / 5.0f,
                             Map::RADIUS),
        sphericalToCartesian(atan(0.5f), (idx + 1.0f) * two_pi<float>() / 5.0f,
                             Map::RADIUS)});

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
  vec3 half0 = (vertices[1] + vertices[2]) / 2.0f;
  vec3 half1 = (vertices[0] + vertices[2]) / 2.0f;
  vec3 half2 = (vertices[0] + vertices[1]) / 2.0f;
  if (distance(vertices[0], vertices[1]) <= Map::MAX_TILE_SIZE) {
    children_[0] =
        make_unique<LeafNode>(array<vec3, 3>{vertices[0], half2, half1});
    children_[1] =
        make_unique<LeafNode>(array<vec3, 3>{vertices[1], half0, half2});
    children_[2] =
        make_unique<LeafNode>(array<vec3, 3>{vertices[2], half1, half0});
    children_[3] = make_unique<LeafNode>(array<vec3, 3>{half0, half1, half2});
  } else {
    children_[0] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices[0], half2, half1});
    children_[1] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices[1], half0, half2});
    children_[2] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices[2], half1, half0});
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
