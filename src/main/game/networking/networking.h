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

#ifndef AIREWAR_GAME_NETWORKING_NETWORKING_H_
#define AIREWAR_GAME_NETWORKING_NETWORKING_H_

#include <sodium.h>

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace airewar::game::networking {
constexpr uint16_t PORT = 10512;

class Connection {
 public:
  Connection() noexcept = default;
  Connection(Connection const &) noexcept = delete;
  Connection(Connection &&) noexcept = default;

  virtual ~Connection() noexcept = default;

  Connection &operator=(Connection const &) noexcept = delete;
  Connection &operator=(Connection &&) noexcept = default;

  Connection &operator<<(uint8_t data);
  Connection &operator<<(uint16_t data);
  Connection &operator<<(uint32_t data);
  Connection &operator<<(uint64_t data);
  Connection &operator<<(int8_t data);
  Connection &operator<<(int16_t data);
  Connection &operator<<(int32_t data);
  Connection &operator<<(int64_t data);
  Connection &operator<<(float data);
  Connection &operator<<(double data);
  Connection &operator<<(char8_t data);
  Connection &operator<<(char32_t data);
  Connection &operator<<(std::u8string data);
  Connection &operator<<(std::u32string data);
  Connection &operator>>(uint8_t &data);
  Connection &operator>>(uint16_t &data);
  Connection &operator>>(uint32_t &data);
  Connection &operator>>(uint64_t &data);
  Connection &operator>>(int8_t &data);
  Connection &operator>>(int16_t &data);
  Connection &operator>>(int32_t &data);
  Connection &operator>>(int64_t &data);
  Connection &operator>>(float &data);
  Connection &operator>>(double &data);
  Connection &operator>>(char8_t &data);
  Connection &operator>>(char32_t &data);
  Connection &operator>>(std::u8string &data);
  Connection &operator>>(std::u32string &data);

  Connection &flush();

  static std::unique_ptr<Connection> makeClient(std::string const &host,
                                                uint16_t port,
                                                std::string const &password);

 protected:
  virtual void sendRaw(void const *data, size_t length) = 0;
  virtual void recvRaw(void *data, size_t length) = 0;

  void handshake(std::string const &password);

 private:
  std::list<uint8_t> sendBuf;
  std::list<uint8_t> recvBuf;

  crypto_secretstream_xchacha20poly1305_state sendState;
  crypto_secretstream_xchacha20poly1305_state recvState;

  void send();
  void recv();

  void wait(size_t n);
};

class Server {
 public:
  Server() noexcept = default;
  Server(Server const &) noexcept = delete;
  Server(Server &&) noexcept = default;

  virtual ~Server() noexcept = default;

  Server &operator=(Server const &) noexcept = delete;
  Server &operator=(Server &&) noexcept = default;

  virtual std::unique_ptr<Connection> accept() = 0;

  static std::unique_ptr<Server> makeServer(uint16_t port,
                                            std::string const &password);
};
}  // namespace airewar::game::networking

#endif  // AIREWAR_GAME_NETWORKING_NETWORKING_H_
