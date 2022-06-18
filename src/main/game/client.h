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

#ifndef AIREWAR_GAME_CLIENT_H_
#define AIREWAR_GAME_CLIENT_H_

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "game/map.h"
#include "game/networking/networking.h"

namespace airewar::game {
class Client final {
 public:
  Map map;

  enum class State {
    STARTING,
    GENERATING_MAP,
    RUNNING,
    ERROR,
  };
  std::atomic<State> state;
  std::string errorMessage;

  Client(std::u32string const &address,
         std::u32string const &password) noexcept;
  Client(Client const &) noexcept = delete;
  Client(Client &&) noexcept = delete;

  ~Client() noexcept;

  Client &operator=(Client const &) noexcept = delete;
  Client &operator=(Client &&) noexcept = delete;

 private:
  std::u32string address;
  std::u32string password;
  std::atomic_bool stop;
  std::unique_ptr<networking::Connection> connection;

  std::thread thread;

  void run() noexcept;
};

extern std::unique_ptr<Client> client;
}  // namespace airewar::game

#endif  // AIREWAR_GAME_CLIENT_H_
