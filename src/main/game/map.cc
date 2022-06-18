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
#include <list>
#include <random>
#include <thread>
#include <utility>
#include <vector>

#include "util/geometry.h"

using namespace std;
using namespace glm;
using namespace airewar::util;
using namespace std::execution;

namespace airewar::game {
namespace {
float angleBetween(vec3 const &a, vec3 const &b) {
  return acos(glm::clamp(dot(normalize(a), normalize(b)), -1.0f, 1.0f));
}
}  // namespace

Map::Tile::Tile(vec3 const &centroid_) noexcept : centroid(centroid_) {}

Map::Plate::Plate(bool major_, bool continental_, Tile &center_) noexcept
    : major(major_), continental(continental_), center(center_) {}

void Map::generate(uint64_t seed) noexcept {
  seed = seed;
  root = make_unique<IcosaNode>();
  mt19937_64 rng(seed);

  // step 1: generate plates and heightmap (see
  // https://www.youtube.com/watch?v=x_Tn66PvTn4)

  // step 1.1: generate plate boundaries

  uniform_real_distribution<float> zeroToOne(0.0f, 1.0f);
  uniform_int_distribution<size_t> majorPlateSelector(0, NUM_MAJOR_PLATES - 1);

  // step 1.1.1: generate major plates
  while (plates.size() < NUM_MAJOR_PLATES) {
    // step 1.1.1.1: generate random point
    float lon = two_pi<float>() * zeroToOne(rng);
    float lat = acos(2 * zeroToOne(rng) - 1) - half_pi<float>();
    vec3 attempt = sphericalToCartesian(lat, lon, RADIUS);

    // step 1.1.1.2: require that point be far enough away from existing points
    if (any_of(plates.begin(), plates.end(), [&attempt](Plate const &plate) {
          return angleBetween(attempt, plate.center.centroid) <
                 MIN_MAJOR_PLATE_ANGLE;
        }))
      continue;

    // step 1.1.1.3: place seed
    plates.emplace_back(true, plates.size() % 2 == 0, (*root)[attempt]);
  }

  // step 1.1.2: generate minor plates
  while (plates.size() < NUM_MAJOR_PLATES + NUM_MINOR_PLATES) {
    // step 1.1.2.1: generate random point
    float lon = two_pi<float>() * zeroToOne(rng);
    float lat = acos(2 * zeroToOne(rng) - 1) - half_pi<float>();
    vec3 attempt = sphericalToCartesian(lat, lon, RADIUS);

    // step 1.1.1.2: require that point be far enough away from existing points
    if (any_of(plates.begin(), plates.end(), [&attempt](Plate const &plate) {
          if (plate.major)
            return angleBetween(attempt, plate.center.centroid) <
                   MIN_MAJOR_PLATE_ANGLE;
          else
            return angleBetween(attempt, plate.center.centroid) <
                   MIN_MINOR_PLATE_ANGLE;
        }))
      continue;

    // step 1.1.2.3: place seed
    plates.emplace_back(false, plates.size() % 3 == 0, (*root)[attempt]);
  }

  // step 1.1.3: attach tiles to plates
  root->forEach([this](Map::Tile &tile) {
    tile.plate = &*min_element(
        plates.begin(), plates.end(), [&tile](Plate const &a, Plate const &b) {
          float distance1 = angleBetween(tile.centroid, a.center.centroid);
          float distance2 = angleBetween(tile.centroid, b.center.centroid);
          if (a.major) distance1 /= MAJOR_PLATE_SIZE_MULTIPLIER;
          if (b.major) distance2 /= MAJOR_PLATE_SIZE_MULTIPLIER;

          return distance1 < distance2;
        });
  });

  // step 1.2: set plate motion (transform and rotational)

  // TODO

  // step 1.3: generate heightmap

  // step 1.3.1: continents + coastal islands

  // TODO

  // step 1.3.2: plate boundary islands

  // TODO

  // step 1.3.3: hotspot island arcs

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

  // step 4.2.4: polar climates

  // TODO

  // step 5: generate rivers (see
  // https://www.youtube.com/watch?v=cqMiMKnYk5E)

  // step 5.1: drainage basins

  // TODO

  // step 5.2: river basins

  // TODO

  // step 5.3: primary rivers

  // TODO

  // step 5.4: tributaries

  // TODO

  // step 6: calculate resource distribution (see
  // https://www.youtube.com/watch?v=b9qvQspSbWc)

  // step 6.1: calculate historical heightmap

  // TODO

  // step 6.2: calculate historical tectonic plates

  // TODO

  // step 6.3: calculate historical currents

  // TODO

  // step 6.4: calculate historical biomes

  // TODO

  // step 6.5: place resources

  // step 6.5.1: coal

  // TODO

  // step 6.5.2: oil

  // TODO

  // step 6.5.3: ores

  // TODO

  // step 7: // TODO
}

uint64_t Map::getSeed() const noexcept { return seed; }

Map::Tile &Map::operator[](glm::vec3 const &ray) noexcept {
  return (*root)[ray];
}

size_t Map::countTiles() const noexcept {
  vector<thread> threads;
  list<size_t> counts;

  for (unique_ptr<TriangularNode> &child : root->children) {
    counts.push_back(0);
    threads.emplace_back(
        [](unique_ptr<TriangularNode> &child, size_t &count) {
          child->forEach([&count](Tile &) { ++count; });
        },
        ref(child), ref(counts.back()));
  }

  for_each(threads.begin(), threads.end(), [](thread &t) { return t.join(); });

  return accumulate(counts.begin(), counts.end(), 0);
}

Map::IcosaNode::IcosaNode() noexcept : children() {
  // NOTE: assumes at least one level of triangle nodes before the leaf nodes

  vector<thread> threads;

  // north polar cap
  for (size_t idx : {0, 1, 2, 3, 4})
    threads.emplace_back(
        [this](size_t idx) {
          children[idx] = make_unique<TriangleNode>(array<vec3, 3>{
              sphericalToCartesian(half_pi<float>(), 0.0f, Map::RADIUS),
              sphericalToCartesian(atan(0.5f), idx * two_pi<float>() / 5.0f,
                                   Map::RADIUS),
              sphericalToCartesian(atan(0.5f),
                                   (idx + 1) * two_pi<float>() / 5.0f,
                                   Map::RADIUS)});
        },
        idx);

  // south polar cap
  for (size_t idx : {0, 1, 2, 3, 4})
    threads.emplace_back(
        [this](size_t idx) {
          children[idx + 5] = make_unique<TriangleNode>(array<vec3, 3>{
              sphericalToCartesian(-half_pi<float>(), 0.0f, Map::RADIUS),
              sphericalToCartesian(-atan(0.5f),
                                   (idx + 1.5f) * two_pi<float>() / 5.0f,
                                   Map::RADIUS),
              sphericalToCartesian(-atan(0.5f),
                                   (idx + 0.5f) * two_pi<float>() / 5.0f,
                                   Map::RADIUS)});
        },
        idx);

  // pointy north half of equator
  for (size_t idx : {0, 1, 2, 3, 4})
    threads.emplace_back(
        [this](size_t idx) {
          children[idx + 10] = make_unique<TriangleNode>(array<vec3, 3>{
              sphericalToCartesian(atan(0.5f), idx * two_pi<float>() / 5.0f,
                                   Map::RADIUS),
              sphericalToCartesian(-atan(0.5f),
                                   (idx - 0.5f) * two_pi<float>() / 5.0f,
                                   Map::RADIUS),
              sphericalToCartesian(-atan(0.5f),
                                   (idx + 0.5f) * two_pi<float>() / 5.0f,
                                   Map::RADIUS)});
        },
        idx);

  // flat north half of equator
  for (size_t idx : {0, 1, 2, 3, 4})
    threads.emplace_back(
        [this](size_t idx) {
          children[idx + 15] = make_unique<TriangleNode>(array<vec3, 3>{
              sphericalToCartesian(atan(0.5f), idx * two_pi<float>() / 5.0f,
                                   Map::RADIUS),
              sphericalToCartesian(-atan(0.5f),
                                   (idx + 0.5f) * two_pi<float>() / 5.0f,
                                   Map::RADIUS),
              sphericalToCartesian(atan(0.5f),
                                   (idx + 1.0f) * two_pi<float>() / 5.0f,
                                   Map::RADIUS)});
        },
        idx);

  for_each(threads.begin(), threads.end(), [](thread &t) { t.join(); });

  projectOntoSphere();
}

Map::Tile &Map::IcosaNode::operator[](vec3 const &ray) noexcept {
  array<unique_ptr<TriangularNode>, 20>::iterator found =
      find_if(par_unseq, children.begin(), children.end(),
              [&ray](unique_ptr<TriangularNode> const &child) {
                return child->intersectsRay(ray);
              });
  if (found != children.cend()) {
    return (**found)[ray];
  } else {
    return (**min_element(children.begin(), children.end(),
                          [&ray](unique_ptr<TriangularNode> const &a,
                                 unique_ptr<TriangularNode> const &b) {
                            return angleBetween(ray, a->centroid) <
                                   angleBetween(ray, b->centroid);
                          }))[ray];
  }
}

Map::Tile const &Map::IcosaNode::operator[](vec3 const &ray) const noexcept {
  array<unique_ptr<TriangularNode>, 20>::const_iterator found =
      find_if(par_unseq, children.cbegin(), children.cend(),
              [&ray](unique_ptr<TriangularNode> const &child) {
                return child->intersectsRay(ray);
              });
  if (found != children.cend()) {
    return (**found)[ray];
  } else {
    return (**min_element(children.cbegin(), children.cend(),
                          [&ray](unique_ptr<TriangularNode> const &a,
                                 unique_ptr<TriangularNode> const &b) {
                            return angleBetween(ray, a->centroid) <
                                   angleBetween(ray, b->centroid);
                          }))[ray];
  }
}

void Map::IcosaNode::projectOntoSphere() noexcept {
  vector<thread> threads;

  for (unique_ptr<TriangularNode> &child : children) {
    threads.emplace_back(
        [](unique_ptr<TriangularNode> &child) {
          return child->projectOntoSphere();
        },
        ref(child));
  }

  for_each(threads.begin(), threads.end(), [](thread &t) { return t.join(); });
}

bool Map::IcosaNode::intersectsRay(vec3 const &ray) const noexcept {
  return true;
}

void Map::IcosaNode::forEach(function<void(Map::Tile &)> const &f) noexcept {
  vector<thread> threads;

  for (unique_ptr<TriangularNode> &child : children) {
    threads.emplace_back(
        [&f](unique_ptr<TriangularNode> &child) { return child->forEach(f); },
        ref(child));
  }

  for_each(threads.begin(), threads.end(), [](thread &t) { return t.join(); });
}

Map::TriangularNode::TriangularNode(array<vec3, 3> const &vertices) noexcept
    : vertices(vertices),
      centroid(RADIUS * normalize(accumulate(vertices.begin(), vertices.end(),
                                             vec3{0.0f, 0.0f, 0.0f}) /
                                  3.0f)) {
  assert(0.999f < angleBetween(vertices[0] - vertices[2],
                               vertices[1] - vertices[2]) /
                      (pi<float>() / 3.0f) &&
         angleBetween(vertices[0] - vertices[2], vertices[1] - vertices[2]) /
                 (pi<float>() / 3.0f) <
             1.001f);
  assert(0.999f < angleBetween(vertices[1] - vertices[0],
                               vertices[2] - vertices[0]) /
                      (pi<float>() / 3.0f) &&
         angleBetween(vertices[1] - vertices[0], vertices[2] - vertices[0]) /
                 (pi<float>() / 3.0f) <
             1.001f);
  assert(0.999f < angleBetween(vertices[2] - vertices[1],
                               vertices[0] - vertices[1]) /
                      (pi<float>() / 3.0f) &&
         angleBetween(vertices[2] - vertices[1], vertices[0] - vertices[1]) /
                 (pi<float>() / 3.0f) <
             1.001f);

  assert(0.999f < distance(vertices[0], vertices[2]) /
                      distance(vertices[1], vertices[2]) &&
         distance(vertices[0], vertices[2]) /
                 distance(vertices[1], vertices[2]) <
             1.001f);
  assert(0.999f < distance(vertices[1], vertices[0]) /
                      distance(vertices[2], vertices[0]) &&
         distance(vertices[1], vertices[0]) /
                 distance(vertices[2], vertices[0]) <
             1.001f);
  assert(0.999f < distance(vertices[2], vertices[1]) /
                      distance(vertices[0], vertices[1]) &&
         distance(vertices[2], vertices[1]) /
                 distance(vertices[0], vertices[1]) <
             1.001f);
}

void Map::TriangularNode::projectOntoSphere() noexcept {
  for (auto &vertex : vertices) vertex = RADIUS * normalize(vertex);
}

bool Map::TriangularNode::intersectsRay(vec3 const &ray) const noexcept {
  return rayIntersectsTriangle(ray, vertices);
}

Map::TriangleNode::TriangleNode(array<vec3, 3> const &vertices) noexcept
    : TriangularNode(vertices), children() {
  vec3 half0 = (vertices[1] + vertices[2]) / 2.0f;
  vec3 half1 = (vertices[0] + vertices[2]) / 2.0f;
  vec3 half2 = (vertices[0] + vertices[1]) / 2.0f;
  if (distance(vertices[0], vertices[1]) <= Map::MAX_TILE_SIZE) {
    children[0] =
        make_unique<LeafNode>(array<vec3, 3>{vertices[0], half2, half1});
    children[1] =
        make_unique<LeafNode>(array<vec3, 3>{vertices[1], half0, half2});
    children[2] =
        make_unique<LeafNode>(array<vec3, 3>{vertices[2], half1, half0});
    children[3] = make_unique<LeafNode>(array<vec3, 3>{half0, half1, half2});
  } else {
    children[0] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices[0], half2, half1});
    children[1] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices[1], half0, half2});
    children[2] =
        make_unique<TriangleNode>(array<vec3, 3>{vertices[2], half1, half0});
    children[3] =
        make_unique<TriangleNode>(array<vec3, 3>{half0, half1, half2});
  }
}

Map::Tile &Map::TriangleNode::operator[](vec3 const &ray) noexcept {
  array<unique_ptr<TriangularNode>, 4>::iterator found =
      find_if(par_unseq, children.begin(), children.end(),
              [&ray](unique_ptr<TriangularNode> const &child) {
                return child->intersectsRay(ray);
              });
  if (found != children.end()) {
    return (**found)[ray];
  } else {
    return (**min_element(children.begin(), children.end(),
                          [&ray](unique_ptr<TriangularNode> const &a,
                                 unique_ptr<TriangularNode> const &b) {
                            return angleBetween(ray, a->centroid) <
                                   angleBetween(ray, b->centroid);
                          }))[ray];
  }
}

Map::Tile const &Map::TriangleNode::operator[](vec3 const &ray) const noexcept {
  assert(intersectsRay(ray));
  array<unique_ptr<TriangularNode>, 4>::const_iterator found =
      find_if(par_unseq, children.cbegin(), children.cend(),
              [&ray](unique_ptr<TriangularNode> const &child) {
                return child->intersectsRay(ray);
              });
  if (found != children.cend()) {
    return (**found)[ray];
  } else {
    return (**min_element(children.cbegin(), children.cend(),
                          [&ray](unique_ptr<TriangularNode> const &a,
                                 unique_ptr<TriangularNode> const &b) {
                            return angleBetween(ray, a->centroid) <
                                   angleBetween(ray, b->centroid);
                          }))[ray];
  }
}

void Map::TriangleNode::projectOntoSphere() noexcept {
  TriangularNode::projectOntoSphere();
  for (auto &child : children) child->projectOntoSphere();
}

void Map::TriangleNode::forEach(function<void(Map::Tile &)> const &f) noexcept {
  for (auto &child : children) child->forEach(f);
}

Map::LeafNode::LeafNode(array<vec3, 3> const &vertices) noexcept
    : TriangularNode(vertices), tile(centroid) {
  assert(distance(vertices[0], vertices[1]) <= Map::MAX_TILE_SIZE);
  assert(distance(vertices[1], vertices[2]) <= Map::MAX_TILE_SIZE);
  assert(distance(vertices[2], vertices[0]) <= Map::MAX_TILE_SIZE);
  assert(angleBetween(vertices[0], vertices[1]) * 6'371'000.0f <=
         Map::MAX_TILE_SIZE);
  assert(angleBetween(vertices[1], vertices[2]) * 6'371'000.0f <=
         Map::MAX_TILE_SIZE);
  assert(angleBetween(vertices[2], vertices[0]) * 6'371'000.0f <=
         Map::MAX_TILE_SIZE);
}

Map::Tile &Map::LeafNode::operator[](vec3 const &) noexcept { return tile; }

Map::Tile const &Map::LeafNode::operator[](vec3 const &) const noexcept {
  return tile;
}

void Map::LeafNode::projectOntoSphere() noexcept {
  TriangularNode::projectOntoSphere();

  assert(distance(vertices[0], vertices[1]) <= Map::MAX_TILE_SIZE);
  assert(distance(vertices[1], vertices[2]) <= Map::MAX_TILE_SIZE);
  assert(distance(vertices[2], vertices[0]) <= Map::MAX_TILE_SIZE);
  assert(angleBetween(vertices[0], vertices[1]) * 6'371'000.0f <=
         Map::MAX_TILE_SIZE);
  assert(angleBetween(vertices[1], vertices[2]) * 6'371'000.0f <=
         Map::MAX_TILE_SIZE);
  assert(angleBetween(vertices[2], vertices[0]) * 6'371'000.0f <=
         Map::MAX_TILE_SIZE);
}

void Map::LeafNode::forEach(function<void(Map::Tile &)> const &f) noexcept {
  return f(tile);
}
}  // namespace airewar::game
