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

#include "util/exceptions/initException.h"

#include <sstream>
#include <string>

using namespace std;

namespace airewar::util::exceptions {
InitException::InitException(string const &title,
                             string const &message) noexcept
    : title(title), message(message), whatString([&title, &message]() {
        stringstream ss;
        ss << title << ": " << message;
        return ss.str();
      }()) {}

InitException::InitException(string const &message) noexcept
    : title(message), message(message), whatString(message) {}

char const *InitException::what() const noexcept { return whatString.c_str(); }

string const &InitException::getTitle() const noexcept { return title; }

string const &InitException::getMessage() const noexcept { return message; }
}  // namespace airewar::util::exceptions
