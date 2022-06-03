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

#include "util/coordinates.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "glm/gtc/constants.hpp"

using namespace glm;
using namespace airewar::util;
using namespace Catch;

TEST_CASE("spherical to cartesian and back is the identity",
          "[util] [coordinates]") {
  float lat = GENERATE(take(100, random(-half_pi<float>(), half_pi<float>())));
  float lon = GENERATE(take(100, random(0.0f, two_pi<float>())));
  float radius = GENERATE(1.0f, 6'371'000.0f);

  vec3 spherical = vec3{lat, lon, radius};
  vec3 cartesian = sphericalToCartesian(lat, lon, radius);
  vec3 spherical_back = cartesianToSpherical(cartesian);

  SECTION("equality test") {
    REQUIRE(spherical.x == Approx(spherical_back.x));
    REQUIRE(spherical.y == Approx(spherical_back.y));
    REQUIRE(spherical.z == Approx(spherical_back.z));
  }
}