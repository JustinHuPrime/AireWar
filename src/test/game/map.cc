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

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <glm/gtc/constants.hpp>
#include <limits>
#include <thread>
#include <vector>

#include "stb_image_write.h"
#include "util/geometry.h"

using namespace std;
using namespace airewar::game;
using namespace glm;
using namespace airewar::util;
using namespace Catch;

TEST_CASE("map generates", "[game][map][.long]") {
  Map map;
  map.generate(
      GENERATE(take(10, random(0UL, numeric_limits<uint64_t>().max()))));

  REQUIRE(map.plates.size() == 18);

  REQUIRE(map.countTiles() % 20 == 0);
  float integral;
  float fractional = modf(log(map.countTiles() / 20) / log(4), &integral);
  REQUIRE(fractional == Approx(0.0f));
}

constexpr int IMAGE_WIDTH = 4000;
constexpr int IMAGE_HEIGHT = 2000;
constexpr int IMAGE_CHANNELS = 3;

TEST_CASE("generate png maps", "[game][map][.long]") {
  Map map;
  map.generate(
      GENERATE(take(1, random(0UL, numeric_limits<uint64_t>().max()))));

  unordered_map<Map::Plate const *, tuple<uint8_t, uint8_t, uint8_t>>
      plateColours = {
          {&map.plates[0], {255, 0, 0}},  {&map.plates[1], {0, 0, 255}},
          {&map.plates[2], {171, 85, 0}}, {&map.plates[3], {0, 0, 228}},
          {&map.plates[4], {85, 171, 0}}, {&map.plates[5], {0, 0, 199}},
          {&map.plates[6], {0, 255, 0}},  {&map.plates[7], {0, 0, 171}},
          {&map.plates[8], {128, 0, 0}},  {&map.plates[9], {0, 0, 142}},
          {&map.plates[10], {96, 32, 0}}, {&map.plates[11], {0, 0, 114}},
          {&map.plates[12], {64, 64, 0}}, {&map.plates[13], {0, 0, 85}},
          {&map.plates[14], {32, 96, 0}}, {&map.plates[15], {0, 0, 57}},
          {&map.plates[16], {0, 128, 0}}, {&map.plates[17], {0, 0, 28}},
      };

  unique_ptr<uint8_t[]> pixels =
      make_unique<uint8_t[]>(IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_CHANNELS);

  vector<thread> threads;

  for (size_t cnt = 0; cnt < 20; ++cnt) {
    threads.emplace_back(
        [&pixels, &map, &plateColours](size_t startY) {
          for (size_t y = startY; y < startY + IMAGE_HEIGHT / 20; ++y) {
            for (size_t x = 0; x < IMAGE_WIDTH; ++x) {
              float lat = -pi<float>() / IMAGE_HEIGHT * y + half_pi<float>();
              float lon = two_pi<float>() / IMAGE_WIDTH * x;
              Map::Tile &tile =
                  map[sphericalToCartesian(lat, lon, Map::RADIUS)];
              pixels[(y * IMAGE_WIDTH + x) * IMAGE_CHANNELS] =
                  get<0>(plateColours.find(tile.plate)->second);
              pixels[(y * IMAGE_WIDTH + x) * IMAGE_CHANNELS + 1] =
                  get<1>(plateColours.find(tile.plate)->second);
              pixels[(y * IMAGE_WIDTH + x) * IMAGE_CHANNELS + 2] =
                  get<2>(plateColours.find(tile.plate)->second);
            }
          }
        },
        IMAGE_HEIGHT / 20 * cnt);
  }

  for_each(threads.begin(), threads.end(), [](thread &t) { return t.join(); });

  for_each(
      map.plates.begin(), map.plates.end(), [&pixels](Map::Plate const &plate) {
        vec3 center = cartesianToSpherical(plate.center.centroid);
        float lat = center.x;
        float lon = center.y;

        int y =
            roundeven((lat - half_pi<float>()) * IMAGE_HEIGHT / -pi<float>());
        int x = roundeven(lon * IMAGE_WIDTH / two_pi<float>());

        for (int offsetY = -10; offsetY <= 10; ++offsetY) {
          for (int offsetX = -10; offsetX <= 10; ++offsetX) {
            pixels[((y + offsetY) * IMAGE_WIDTH + x + offsetX) *
                   IMAGE_CHANNELS] = 255;
            pixels[((y + offsetY) * IMAGE_WIDTH + x + offsetX) *
                       IMAGE_CHANNELS +
                   1] = 0;
            pixels[((y + offsetY) * IMAGE_WIDTH + x + offsetX) *
                       IMAGE_CHANNELS +
                   2] = 255;
          }
        }
      });

  REQUIRE(stbi_write_png("tectonic_plates.png", IMAGE_WIDTH, IMAGE_HEIGHT,
                         IMAGE_CHANNELS, pixels.get(), 0) != 0);
}