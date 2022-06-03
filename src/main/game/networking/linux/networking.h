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

#ifdef __linux__

#ifndef AIREWAR_GAME_NETWORKING_LINUX_NETWORKING_H_
#define AIREWAR_GAME_NETWORKING_LINUX_NETWORKING_H_

#include <memory>
#include <string>

#include "game/networking/networking.h"

namespace airewar::game::networking::linux {
class FD final {
 public:
  FD() noexcept;
  explicit FD(int fd) noexcept;
  FD(FD const &) noexcept = delete;
  FD(FD &&) noexcept;

  ~FD() noexcept;

  FD &operator=(FD const &) noexcept = delete;
  FD &operator=(FD &&) noexcept;

  bool operator!() const noexcept;
  operator bool() const noexcept;

  int get() noexcept;

 private:
  int fd_;
};

class Connection : public airewar::game::networking::Connection {
 public:
  explicit Connection(FD fd, std::atomic_bool const &stop) noexcept;
  Connection(std::string const &address, uint16_t port,
             std::atomic_bool const &stop);
  Connection(Connection const &) noexcept = delete;
  Connection(Connection &&) noexcept = default;

  ~Connection() noexcept override = default;

  Connection &operator=(Connection const &) noexcept = delete;
  Connection &operator=(Connection &&) noexcept = default;

 protected:
  void sendRaw(void const *data, size_t length) override;
  void recvRaw(void *data, size_t length) override;

 private:
  FD fd_;
  std::atomic_bool const &stop_;
};

class Server : public airewar::game::networking::Server {
 public:
  explicit Server(uint16_t port, std::string const &password,
                  std::atomic_bool const &stop);
  Server(Server const &) noexcept = delete;
  Server(Server &&) noexcept = default;

  ~Server() noexcept override = default;

  Server &operator=(Server const &) noexcept = delete;
  Server &operator=(Server &&) noexcept = default;

  std::unique_ptr<airewar::game::networking::Connection> accept() override;

 private:
  FD fd_;
  std::string const &password_;
  std::atomic_bool const &stop_;
};
}  // namespace airewar::game::networking::linux

#endif  // AIREWAR_GAME_NETWORKING_LINUX_NETWORKING_H_

#endif  // __linux__
