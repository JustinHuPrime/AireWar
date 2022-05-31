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

#ifndef AIREWAR_UI_FREETYPE_H_
#define AIREWAR_UI_FREETYPE_H_

#include <memory>

#include "ft2build.h"
#include FT_FREETYPE_H

namespace airewar::ui {
class FreeType final {
 public:
  FreeType();
  FreeType(FreeType const &) noexcept = delete;
  FreeType(FreeType &&) noexcept = delete;

  ~FreeType() noexcept = default;

  FreeType &operator=(FreeType const &) noexcept = delete;
  FreeType &operator=(FreeType &&) noexcept = delete;

  FT_Library get() noexcept;
  FT_Library const get() const noexcept;

 private:
  std::unique_ptr<std::remove_pointer<FT_Library>::type,
                  decltype(&FT_Done_FreeType)>
      library_;
};

extern std::unique_ptr<FreeType> freetype;
}  // namespace airewar::ui

#endif  // AIREWAR_UI_FREETYPE_H_
