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

#include "game/networking/networking.h"

#include <sodium.h>

#include <algorithm>
#include <memory>

#include "util/exceptions/formatException.h"
#include "util/exceptions/socketException.h"

#ifdef __linux__
#include "game/networking/linux/networking.h"
#elif
#error "operating system not supported/recognized"
#endif

using namespace std;
using namespace airewar::util::exceptions;

namespace airewar::game::networking {
Connection &Connection::operator<<(uint8_t data) {
  sendBuf.push_back('b');
  sendBuf.push_back(data);
  return *this;
}

Connection &Connection::operator<<(uint16_t data) {
  sendBuf.push_back('s');
  sendBuf.push_back(data >> 8 & 0xff);
  sendBuf.push_back(data >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(uint32_t data) {
  sendBuf.push_back('i');
  sendBuf.push_back(data >> 24 & 0xff);
  sendBuf.push_back(data >> 16 & 0xff);
  sendBuf.push_back(data >> 8 & 0xff);
  sendBuf.push_back(data >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(uint64_t data) {
  sendBuf.push_back('l');
  sendBuf.push_back(data >> 56 & 0xff);
  sendBuf.push_back(data >> 48 & 0xff);
  sendBuf.push_back(data >> 40 & 0xff);
  sendBuf.push_back(data >> 32 & 0xff);
  sendBuf.push_back(data >> 24 & 0xff);
  sendBuf.push_back(data >> 16 & 0xff);
  sendBuf.push_back(data >> 8 & 0xff);
  sendBuf.push_back(data >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(int8_t data) {
  union {
    uint8_t u;
    int8_t i;
  } u = {.i = data};
  sendBuf.push_back('B');
  sendBuf.push_back(u.u);
  return *this;
}

Connection &Connection::operator<<(int16_t data) {
  union {
    uint16_t u;
    int16_t i;
  } u = {.i = data};
  sendBuf.push_back('S');
  sendBuf.push_back(u.u >> 8 & 0xff);
  sendBuf.push_back(u.u >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(int32_t data) {
  union {
    uint32_t u;
    int32_t i;
  } u = {.i = data};
  sendBuf.push_back('I');
  sendBuf.push_back(u.u >> 24 & 0xff);
  sendBuf.push_back(u.u >> 16 & 0xff);
  sendBuf.push_back(u.u >> 8 & 0xff);
  sendBuf.push_back(u.u >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(int64_t data) {
  union {
    uint64_t u;
    int64_t i;
  } u = {.i = data};
  sendBuf.push_back('L');
  sendBuf.push_back(u.u >> 56 & 0xff);
  sendBuf.push_back(u.u >> 48 & 0xff);
  sendBuf.push_back(u.u >> 40 & 0xff);
  sendBuf.push_back(u.u >> 32 & 0xff);
  sendBuf.push_back(u.u >> 24 & 0xff);
  sendBuf.push_back(u.u >> 16 & 0xff);
  sendBuf.push_back(u.u >> 8 & 0xff);
  sendBuf.push_back(u.u >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(float data) {
  union {
    uint32_t u;
    float f;
  } u = {.f = data};
  sendBuf.push_back('F');
  sendBuf.push_back(u.u >> 24 & 0xff);
  sendBuf.push_back(u.u >> 16 & 0xff);
  sendBuf.push_back(u.u >> 8 & 0xff);
  sendBuf.push_back(u.u >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(double data) {
  union {
    uint64_t u;
    double d;
  } u = {.d = data};
  sendBuf.push_back('D');
  sendBuf.push_back(u.u >> 56 & 0xff);
  sendBuf.push_back(u.u >> 48 & 0xff);
  sendBuf.push_back(u.u >> 40 & 0xff);
  sendBuf.push_back(u.u >> 32 & 0xff);
  sendBuf.push_back(u.u >> 24 & 0xff);
  sendBuf.push_back(u.u >> 16 & 0xff);
  sendBuf.push_back(u.u >> 8 & 0xff);
  sendBuf.push_back(u.u >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(char8_t data) {
  union {
    char8_t c;
    uint8_t u;
  } u = {.c = data};
  sendBuf.push_back('c');
  sendBuf.push_back(u.u);
  return *this;
}

Connection &Connection::operator<<(char32_t data) {
  union {
    char32_t c;
    uint32_t u;
  } u = {.c = data};
  sendBuf.push_back('C');
  sendBuf.push_back(u.u >> 24 & 0xff);
  sendBuf.push_back(u.u >> 16 & 0xff);
  sendBuf.push_back(u.u >> 8 & 0xff);
  sendBuf.push_back(u.u >> 0 & 0xff);
  return *this;
}

Connection &Connection::operator<<(std::u8string data) {
  uint64_t length = data.length();
  sendBuf.push_back('u');
  sendBuf.push_back(length >> 56 & 0xff);
  sendBuf.push_back(length >> 48 & 0xff);
  sendBuf.push_back(length >> 40 & 0xff);
  sendBuf.push_back(length >> 32 & 0xff);
  sendBuf.push_back(length >> 24 & 0xff);
  sendBuf.push_back(length >> 16 & 0xff);
  sendBuf.push_back(length >> 8 & 0xff);
  sendBuf.push_back(length >> 0 & 0xff);
  for_each(data.begin(), data.end(), [this](char8_t c) {
    union {
      char8_t c;
      uint8_t u;
    } u = {.c = c};
    sendBuf.push_back(u.u);
  });
  return *this;
}

Connection &Connection::operator<<(std::u32string data) {
  uint64_t length = data.length();
  sendBuf.push_back('U');
  sendBuf.push_back(length >> 56 & 0xff);
  sendBuf.push_back(length >> 48 & 0xff);
  sendBuf.push_back(length >> 40 & 0xff);
  sendBuf.push_back(length >> 32 & 0xff);
  sendBuf.push_back(length >> 24 & 0xff);
  sendBuf.push_back(length >> 16 & 0xff);
  sendBuf.push_back(length >> 8 & 0xff);
  sendBuf.push_back(length >> 0 & 0xff);
  for_each(data.begin(), data.end(), [this](char32_t c) {
    union {
      char32_t c;
      uint32_t u;
    } u = {.c = c};
    sendBuf.push_back(u.u >> 24 & 0xff);
    sendBuf.push_back(u.u >> 16 & 0xff);
    sendBuf.push_back(u.u >> 8 & 0xff);
    sendBuf.push_back(u.u >> 0 & 0xff);
  });
  return *this;
}

Connection &Connection::operator>>(uint8_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'b') throw FormatException("expected uint8_t");

  wait(1);
  data = recvBuf.front();
  recvBuf.pop_front();

  return *this;
}

Connection &Connection::operator>>(uint16_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 's') throw FormatException("expected uint16_t");

  wait(2);
  data = 0;
  data |= static_cast<uint16_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  data |= static_cast<uint16_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  return *this;
}

Connection &Connection::operator>>(uint32_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'i') throw FormatException("expected uint32_t");

  wait(4);
  data = 0;
  data |= static_cast<uint32_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  data |= static_cast<uint32_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  data |= static_cast<uint32_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  data |= static_cast<uint32_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();
  return *this;
}

Connection &Connection::operator>>(uint64_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'l') throw FormatException("expected uint64_t");

  wait(8);
  data = 0;
  data |= static_cast<uint64_t>(recvBuf.front()) << 56;
  recvBuf.pop_front();
  data |= static_cast<uint64_t>(recvBuf.front()) << 48;
  recvBuf.pop_front();
  data |= static_cast<uint64_t>(recvBuf.front()) << 40;
  recvBuf.pop_front();
  data |= static_cast<uint64_t>(recvBuf.front()) << 32;
  recvBuf.pop_front();
  data |= static_cast<uint64_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  data |= static_cast<uint64_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  data |= static_cast<uint64_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  data |= static_cast<uint64_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();
  return *this;
}

Connection &Connection::operator>>(int8_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'B') throw FormatException("expected int8_t");

  wait(1);
  union {
    int8_t s;
    uint8_t u;
  } u = {.u = 0};
  u.u = recvBuf.front();
  recvBuf.pop_front();

  data = u.s;
  return *this;
}

Connection &Connection::operator>>(int16_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'S') throw FormatException("expected int16_t");

  wait(2);
  union {
    int16_t s;
    uint16_t u;
  } u = {.u = 0};
  u.u |= static_cast<uint16_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  u.u |= static_cast<uint16_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  data = u.s;
  return *this;
}

Connection &Connection::operator>>(int32_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'I') throw FormatException("expected int32_t");

  wait(4);
  union {
    int32_t s;
    uint32_t u;
  } u = {.u = 0};
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  data = u.s;
  return *this;
}

Connection &Connection::operator>>(int64_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'L') throw FormatException("expected int64_t");

  wait(8);
  union {
    int64_t s;
    uint64_t u;
  } u = {.u = 0};
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 56;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 48;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 40;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 32;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  data = u.s;
  return *this;
}

Connection &Connection::operator>>(float &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'F') throw FormatException("expected float");

  wait(4);
  union {
    float f;
    uint32_t u;
  } u = {.u = 0};
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  data = u.f;
  return *this;
}

Connection &Connection::operator>>(double &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'D') throw FormatException("expected double");

  wait(8);
  union {
    double d;
    uint64_t u;
  } u = {.u = 0};
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 56;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 48;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 40;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 32;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  u.u |= static_cast<uint64_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  data = u.d;
  return *this;
}

Connection &Connection::operator>>(char8_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'c') throw FormatException("expected char8_t");

  wait(1);
  data = recvBuf.front();
  recvBuf.pop_front();
  return *this;
}

Connection &Connection::operator>>(char32_t &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 'C') throw FormatException("expected char32_t");

  wait(4);
  union {
    char32_t c;
    uint32_t u;
  } u = {.u = 0};
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  u.u |= static_cast<uint32_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  data = u.c;
  return *this;
}

Connection &Connection::operator>>(std::u8string &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 's') throw FormatException("expected std::u8string");

  wait(8);
  uint64_t size = 0;
  size |= static_cast<uint64_t>(recvBuf.front()) << 56;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 48;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 40;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 32;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  wait(size);
  data.clear();
  for (size_t cnt = 0; cnt < size; ++cnt) {
    union {
      char8_t c;
      uint8_t u;
    } u = {.u = recvBuf.front()};
    recvBuf.pop_front();
    data += u.c;
  }
  return *this;
}

Connection &Connection::operator>>(std::u32string &data) {
  wait(1);
  uint8_t type = recvBuf.front();
  recvBuf.pop_front();
  if (type != 's') throw FormatException("expected std::u8string");

  wait(8);
  uint64_t size = 0;
  size |= static_cast<uint64_t>(recvBuf.front()) << 56;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 48;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 40;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 32;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 24;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 16;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 8;
  recvBuf.pop_front();
  size |= static_cast<uint64_t>(recvBuf.front()) << 0;
  recvBuf.pop_front();

  wait(size * 4);
  data.clear();
  for (size_t cnt = 0; cnt < size; ++cnt) {
    union {
      char32_t c;
      uint32_t u;
    } u = {.u = 0};
    u.u |= static_cast<uint32_t>(recvBuf.front()) << 24;
    recvBuf.pop_front();
    u.u |= static_cast<uint32_t>(recvBuf.front()) << 16;
    recvBuf.pop_front();
    u.u |= static_cast<uint32_t>(recvBuf.front()) << 8;
    recvBuf.pop_front();
    u.u |= static_cast<uint32_t>(recvBuf.front()) << 0;
    recvBuf.pop_front();
    data += u.c;
  }
  return *this;
}

Connection &Connection::flush() {
  send();
  return *this;
}

constexpr size_t PACKET_SIZE = 4096;
constexpr size_t PLAINTEXT_SIZE =
    PACKET_SIZE - crypto_secretstream_xchacha20poly1305_ABYTES;
constexpr size_t MESSAGE_SIZE = PLAINTEXT_SIZE - sizeof(uint16_t);

void Connection::send() {
  if (sendBuf.empty()) return;

  unique_ptr<unsigned char[]> plaintext =
      make_unique<unsigned char[]>(PLAINTEXT_SIZE);
  uint16_t len = min(MESSAGE_SIZE, sendBuf.size());
  plaintext[MESSAGE_SIZE + 0] = len >> 8 & 0xff;
  plaintext[MESSAGE_SIZE + 1] = len >> 0 & 0xff;
  for (uint16_t cnt = 0; cnt < len; ++cnt) {
    plaintext[cnt] = sendBuf.front();
    sendBuf.pop_front();
  }
  unique_ptr<unsigned char[]> ciphertext =
      make_unique<unsigned char[]>(PACKET_SIZE);

  crypto_secretstream_xchacha20poly1305_push(
      &sendState, ciphertext.get(), nullptr, plaintext.get(), PLAINTEXT_SIZE,
      nullptr, 0, crypto_secretstream_xchacha20poly1305_TAG_MESSAGE);

  sendRaw(ciphertext.get(), PACKET_SIZE);

  return send();
}

void Connection::recv() {
  unique_ptr<unsigned char[]> ciphertext =
      make_unique<unsigned char[]>(PACKET_SIZE);
  recvRaw(ciphertext.get(), PACKET_SIZE);

  unique_ptr<unsigned char[]> plaintext =
      make_unique<unsigned char[]>(PLAINTEXT_SIZE);

  if (crypto_secretstream_xchacha20poly1305_pull(
          &recvState, plaintext.get(), nullptr, nullptr, ciphertext.get(),
          PACKET_SIZE, nullptr, 0) == -1)
    throw SocketException("Corrupted packet");

  uint16_t len = 0;
  len |= static_cast<uint16_t>(plaintext[MESSAGE_SIZE + 0]) << 8;
  len |= static_cast<uint16_t>(plaintext[MESSAGE_SIZE + 1]) << 0;
  for (uint16_t cnt = 0; cnt < len; ++cnt) {
    recvBuf.push_back(plaintext[cnt]);
  }
}

void Connection::handshake(string const &password) {
  unique_ptr<unsigned char[]> sendSalt =
      make_unique<unsigned char[]>(crypto_pwhash_SALTBYTES);
  randombytes_buf(sendSalt.get(), crypto_pwhash_SALTBYTES);
  sendRaw(sendSalt.get(), crypto_pwhash_SALTBYTES);

  unique_ptr<unsigned char[]> sendKey = make_unique<unsigned char[]>(
      crypto_secretstream_xchacha20poly1305_KEYBYTES);
  if (crypto_pwhash(
          sendKey.get(), crypto_secretstream_xchacha20poly1305_KEYBYTES,
          password.c_str(), password.length(), sendSalt.get(),
          crypto_pwhash_OPSLIMIT_INTERACTIVE,
          crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0) {
    throw SocketException("Failed to generate sending key");
  }

  unique_ptr<unsigned char[]> sendHeader = make_unique<unsigned char[]>(
      crypto_secretstream_xchacha20poly1305_HEADERBYTES);
  crypto_secretstream_xchacha20poly1305_init_push(&sendState, sendHeader.get(),
                                                  sendKey.get());
  sendRaw(sendHeader.get(), crypto_secretstream_xchacha20poly1305_HEADERBYTES);

  unique_ptr<unsigned char[]> recvSalt =
      make_unique<unsigned char[]>(crypto_pwhash_SALTBYTES);
  recvRaw(recvSalt.get(), crypto_pwhash_SALTBYTES);

  unique_ptr<unsigned char[]> recvKey = make_unique<unsigned char[]>(
      crypto_secretstream_xchacha20poly1305_KEYBYTES);
  if (crypto_pwhash(
          recvKey.get(), crypto_secretstream_xchacha20poly1305_KEYBYTES,
          password.c_str(), password.length(), recvSalt.get(),
          crypto_pwhash_OPSLIMIT_INTERACTIVE,
          crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0) {
    throw SocketException("Failed to generate receiving key");
  }

  unique_ptr<unsigned char[]> recvHeader = make_unique<unsigned char[]>(
      crypto_secretstream_xchacha20poly1305_HEADERBYTES);
  recvRaw(recvHeader.get(), crypto_secretstream_xchacha20poly1305_HEADERBYTES);
  if (crypto_secretstream_xchacha20poly1305_init_pull(
          &recvState, recvHeader.get(), recvKey.get()) != 0)
    throw SocketException("Password mismatch");
}

void Connection::wait(size_t n) {
  while (recvBuf.size() < n) recv();
}

#ifdef __linux__
unique_ptr<Connection> makeClient(string const &host, uint16_t port,
                                  string const &password) {
  return make_unique<linux::Connection>(host, port, password);
}

unique_ptr<Server> makeServer(uint16_t port, string const &password) {
  return make_unique<linux::Server>(port, password);
}
#elif
#error "operating system not supported/recognized"
#endif
}  // namespace airewar::game::networking
