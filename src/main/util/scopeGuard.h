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

#ifndef AIREWAR_SCOPEGUARD_H_
#define AIREWAR_SCOPEGUARD_H_

#include <functional>
#include <optional>

namespace airewar::util {
class ScopeGuard final {
 public:
  ScopeGuard() noexcept;
  explicit ScopeGuard(std::function<void()> const &f) noexcept;
  ScopeGuard(ScopeGuard const &) noexcept = delete;
  ScopeGuard(ScopeGuard &&) noexcept = default;

  ~ScopeGuard() noexcept;

  ScopeGuard &operator=(ScopeGuard const &) noexcept = delete;
  ScopeGuard &operator=(ScopeGuard &&) noexcept = default;

  void reset() noexcept;
  void reset(std::function<void()> const &f) noexcept;

 private:
  std::function<void()> f;
};
}  // namespace airewar::util

#endif  // AIREWAR_SCOPEGUARD_H_
