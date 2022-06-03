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

#ifndef AIREWAR_GAME_SERVER_H_
#define AIREWAR_GAME_SERVER_H_

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>

#include "game/map.h"
#include "game/networking/networking.h"

namespace airewar::game {
class Server final {
 public:
  class Connection final {
   public:
    enum class State {
      STARTING,
      RUNNING,
      DONE,
    };
    std::atomic<State> state;

    explicit Connection(
        Server &server,
        std::unique_ptr<networking::Connection> connection) noexcept;
    Connection(Connection const &) noexcept = delete;
    Connection(Connection &&) noexcept = default;

    ~Connection() noexcept;

    Connection &operator=(Connection const &) noexcept = delete;
    Connection &operator=(Connection &&) noexcept = default;

   private:
    Server &server_;
    std::unique_ptr<networking::Connection> connection_;

    std::thread thread_;

    void run() noexcept;
  };

  enum class State {
    STARTING,
    RUNNING,
    ERROR,
  };
  std::atomic<State> state;
  std::string errorMessage;

  Server(std::u32string const &password) noexcept;
  Server(Server const &) noexcept = delete;
  Server(Server &&) noexcept = delete;

  ~Server() noexcept;

  Server &operator=(Server const &) noexcept = delete;
  Server &operator=(Server &&) noexcept = delete;

 private:
  static constexpr size_t NUM_PLAYERS = 2;

  std::string password_;
  std::atomic_bool stop_;
  std::unique_ptr<networking::Server> server_;

  std::mt19937_64 rng_;
  Map map_;

  std::mutex connectionMutex_;
  std::list<Connection> connections_;

  std::thread thread_;

  void run() noexcept;
};

extern std::unique_ptr<Server> server;
}  // namespace airewar::game

#endif  // AIREWAR_GAME_SERVER_H_
