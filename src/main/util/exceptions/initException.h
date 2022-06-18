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

#ifndef AIREWAR_UTIL_EXCEPTIONS_INITEXCEPTION_H_
#define AIREWAR_UTIL_EXCEPTIONS_INITEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace airewar::util::exceptions {
class InitException final : public std::exception {
 public:
  InitException(std::string const &title, std::string const &message) noexcept;
  InitException(std::string const &message) noexcept;
  InitException(InitException const &) noexcept = default;
  InitException(InitException &&) noexcept = default;

  ~InitException() noexcept override = default;

  InitException &operator=(InitException const &) noexcept = default;
  InitException &operator=(InitException &&) noexcept = default;

  char const *what() const noexcept override;
  std::string const &getTitle() const noexcept;
  std::string const &getMessage() const noexcept;

 private:
  std::string title;
  std::string message;
  std::string whatString;
};
}  // namespace airewar::util::exceptions

#endif  // AIREWAR_UTIL_EXCEPTIONS_INITEXCEPTION_H_
