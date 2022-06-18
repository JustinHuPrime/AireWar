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

#ifndef AIREWAR_UTIL_EXCEPTIONS_SOCKETEXCEPTION_H_
#define AIREWAR_UTIL_EXCEPTIONS_SOCKETEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace airewar::util::exceptions {
class SocketException final : public std::exception {
 public:
  SocketException(std::string const &message) noexcept;
  SocketException(SocketException const &) noexcept = default;
  SocketException(SocketException &&) noexcept = default;

  ~SocketException() noexcept override = default;

  SocketException &operator=(SocketException const &) noexcept = default;
  SocketException &operator=(SocketException &&) noexcept = default;

  char const *what() const noexcept override;

  operator std::string() const noexcept;

 private:
  std::string message;
};
}  // namespace airewar::util::exceptions

#endif  // AIREWAR_UTIL_EXCEPTIONS_SOCKETEXCEPTION_H_
