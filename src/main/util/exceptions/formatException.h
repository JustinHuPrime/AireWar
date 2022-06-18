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

#ifndef AIREWAR_UTIL_EXCEPTIONS_FORMATEXCEPTION_H_
#define AIREWAR_UTIL_EXCEPTIONS_FORMATEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace airewar::util::exceptions {
class FormatException final : public std::exception {
 public:
  FormatException(std::string const &message) noexcept;
  FormatException(FormatException const &) noexcept = default;
  FormatException(FormatException &&) noexcept = default;

  ~FormatException() noexcept override = default;

  FormatException &operator=(FormatException const &) noexcept = default;
  FormatException &operator=(FormatException &&) noexcept = default;

  char const *what() const noexcept override;

 private:
  std::string message;
};
}  // namespace airewar::util::exceptions

#endif  // AIREWAR_UTIL_EXCEPTIONS_FORMATEXCEPTION_H_
