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

#include "util/scopeGuard.h"

using namespace std;

namespace airewar::util {
ScopeGuard::ScopeGuard() noexcept : f([]() {}) {}

ScopeGuard::ScopeGuard(std::function<void()> const &f) noexcept : f(f) {}

ScopeGuard::~ScopeGuard() noexcept { f(); }

void ScopeGuard::reset() noexcept {
  f();
  f = []() {};
}

void ScopeGuard::reset(std::function<void()> const &next) noexcept {
  f();
  f = next;
}

}  // namespace airewar::util
