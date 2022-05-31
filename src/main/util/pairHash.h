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

#ifndef AIREWAR_UTIL_PAIRHASH_H_
#define AIREWAR_UTIL_PAIRHASH_H_

#include <utility>

namespace airewar::util {
template <typename T1, typename T2>
struct hash {
  size_t operator()(std::pair<T1, T2> const &p) const noexcept {
    return 3 * std::hash<T1>()(p.first) + std::hash<T2>()(p.second);
  }
};
}  // namespace airewar::util

#endif  // AIREWAR_UTIL_PAIRHASH_H_
