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

#ifndef AIREWAR_UTIL_EXCEPTIONS_STOPFLAG_H_
#define AIREWAR_UTIL_EXCEPTIONS_STOPFLAG_H_

namespace airewar::util::exceptions {
class StopFlag final {
 public:
  StopFlag() noexcept = default;
  StopFlag(StopFlag const &) noexcept = default;
  StopFlag(StopFlag &&) noexcept = default;

  ~StopFlag() noexcept = default;

  StopFlag &operator=(StopFlag const &) noexcept = default;
  StopFlag &operator=(StopFlag &&) noexcept = default;
};
}  // namespace airewar::util::exceptions

#endif  // AIREWAR_UTIL_EXCEPTIONS_STOPFLAG_H_
