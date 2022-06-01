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

#include "game/networking/linux/networking.h"

#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include "util/exceptions/socketException.h"

using namespace std;
using namespace airewar::util::exceptions;

namespace airewar::game::networking::linux {
namespace {
constexpr int POLL_TIMEOUT = 50;
}

FD::FD() noexcept : fd_(-1) {}

FD::FD(int fd) noexcept : fd_(fd) {}

FD::FD(FD &&other) noexcept : fd_(other.fd_) { other.fd_ = -1; }

FD::~FD() noexcept {
  if (fd_ != -1) close(fd_);
}

FD &FD::operator=(FD &&other) noexcept {
  swap(fd_, other.fd_);
  return *this;
}

bool FD::operator!() const noexcept { return fd_ == -1; }

FD::operator bool() const noexcept { return fd_ != -1; }

int FD::get() noexcept { return fd_; }

Connection::Connection(FD fd, string const &password) noexcept
    : fd_(std::move(fd_)) {
  handshake(password);
}

Connection::Connection(string const &address, uint16_t port,
                       string const &password) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV;

  string portStr = to_string(port);
  struct addrinfo *rawResult;
  if (int s = getaddrinfo(address.c_str(), portStr.c_str(), &hints, &rawResult);
      s != 0)
    throw SocketException("Could not get address info: "s + gai_strerror(s));

  unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> result(rawResult,
                                                              freeaddrinfo);
  for (struct addrinfo *curr = result.get(); curr != nullptr && !fd_;
       curr = curr->ai_next) {
    FD attempt(socket(curr->ai_family, curr->ai_socktype | SOCK_NONBLOCK,
                      curr->ai_protocol));
    if (!attempt) continue;

    // start connection attempt
    int retval = connect(attempt.get(), curr->ai_addr, curr->ai_addrlen);
    if (retval == 0) {
      fd_ = move(attempt);
    } else if (retval == -1 && errno == EINPROGRESS) {
      // need to wait a bit and try again
      struct pollfd fds;
      memset(&fds, 0, sizeof(fds));
      fds.fd = attempt.get();
      fds.events = POLLOUT;
      bool connecting = true;
      while (connecting) {
        switch (poll(&fds, 1, POLL_TIMEOUT)) {
          case 0: {
            break;
          }
          case 1: {
            connecting = false;

            int errored;
            socklen_t len = sizeof(errored);
            if (getsockopt(attempt.get(), SOL_SOCKET, SO_ERROR, &errored,
                           &len) == 0 &&
                errored == 0)
              fd_ = move(attempt);

            break;
          }
          default: {
            // poll failed - try a different address
            connecting = false;
            break;
          }
        }
      }
    }
  }

  if (!fd_)
    throw SocketException("Could not connect to server: "s + strerror(errno));

  handshake(password);
}

void Connection::sendRaw(void const *data, size_t length) {
  size_t curr = 0;
  while (curr != length) {
    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = fd_.get();
    fds.events = POLLOUT;

    int pollResult = poll(&fds, 1, POLL_TIMEOUT);
    switch (pollResult) {
      case 0: {
        break;
      }
      case 1: {
        ssize_t sizeRead =
            ::send(fd_.get(), reinterpret_cast<char const *>(data) + curr,
                   length - curr, 0);
        if (sizeRead == -1)
          throw SocketException("Write failed: "s + strerror(errno));

        curr += sizeRead;
        break;
      }
      default: {
        throw SocketException("Poll failed: "s + strerror(errno));
      }
    }
  }
}

void Connection::recvRaw(void *data, size_t length) {
  size_t curr = 0;
  while (curr != length) {
    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = fd_.get();
    fds.events = POLLIN;

    int pollResult = poll(&fds, 1, POLL_TIMEOUT);
    switch (pollResult) {
      case 0: {
        break;
      }
      case 1: {
        ssize_t sizeRead = ::recv(
            fd_.get(), reinterpret_cast<char *>(data) + curr, length - curr, 0);
        if (sizeRead == -1)
          throw SocketException("Read failed: "s + strerror(errno));

        curr += sizeRead;
        break;
      }
      default: {
        throw SocketException("Poll failed: "s + strerror(errno));
      }
    }
  }
}

Server::Server(uint16_t port, string const &password) : password_(password) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

  string portStr = to_string(port);
  struct addrinfo *rawResult;
  if (int s = getaddrinfo(nullptr, portStr.c_str(), &hints, &rawResult); s != 0)
    throw SocketException("Could not get address info: "s + gai_strerror(s));

  unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> result(rawResult,
                                                              freeaddrinfo);
  for (struct addrinfo *curr = result.get(); curr != nullptr && !fd_;
       curr = curr->ai_next) {
    FD attempt(socket(curr->ai_family, curr->ai_socktype | SOCK_NONBLOCK,
                      curr->ai_protocol));
    if (!attempt) continue;

    int one = 1;
    if (setsockopt(attempt.get(), SOL_SOCKET, SO_REUSEADDR, &one,
                   sizeof(one)) != 0)
      continue;

    if (bind(attempt.get(), curr->ai_addr, curr->ai_addrlen) == 0)
      fd_ = move(attempt);
  }

  if (!fd_)
    throw SocketException("Could not bind to port: "s + strerror(errno));

  if (listen(fd_.get(), SOMAXCONN) != 0)
    throw SocketException("Could not listen on port: "s + strerror(errno));
}

unique_ptr<airewar::game::networking::Connection> Server::accept() {
  struct pollfd fds;
  memset(&fds, 0, sizeof(fds));
  fds.fd = fd_.get();
  fds.events = POLLIN;
  switch (poll(&fds, 1, POLL_TIMEOUT)) {
    case 0: {
      return nullptr;
    }
    case 1: {
      return make_unique<Connection>(
          FD(accept4(fd_.get(), nullptr, nullptr, SOCK_NONBLOCK)), password_);
    }
    default: {
      throw SocketException("Poll failed: "s + strerror(errno));
    }
  }
}
}  // namespace airewar::game::networking::linux

#endif  // __linux__
