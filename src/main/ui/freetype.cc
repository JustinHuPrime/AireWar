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

#include "ui/freetype.h"

#include "util/exceptions/initException.h"

using namespace airewar::util::exceptions;

namespace airewar::ui {
FreeType::FreeType()
    : library(
          []() {
            FT_Library library;
            if (FT_Init_FreeType(&library) != 0)
              throw InitException("Could not initialize FreeType");
            return library;
          }(),
          FT_Done_FreeType) {}

FT_Library FreeType::get() noexcept { return library.get(); }

FT_Library const FreeType::get() const noexcept { return library.get(); }

std::unique_ptr<FreeType> freetype;
}  // namespace airewar::ui
