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

#include "game/server.h"

#include <algorithm>
#include <codecvt>
#include <locale>
#include <utility>

#include "options.h"
#include "sodium.h"
#include "util/exceptions/socketException.h"
#include "util/exceptions/stopFlag.h"

using namespace std;
using namespace airewar::util::exceptions;
using namespace airewar::game::networking;

namespace airewar::game {
Server::Connection::Connection(
    Server &server, std::unique_ptr<networking::Connection> socket) noexcept
    : state(State::STARTING),
      server_(server),
      connection_(move(socket)),
      thread_([this]() { return run(); }) {}

Server::Connection::~Connection() { thread_.join(); }

void Server::Connection::run() noexcept {
  try {
    if (!connection_->handshake(server_.password_)) {
      // incorrect password - kill connection
      state = State::DONE;
      return;
    }

    {
      // check for number of players
      scoped_lock lock(server_.connectionMutex_);
      if (count_if(server_.connections_.begin(), server_.connections_.end(),
                   [](Connection const &connection) {
                     return connection.state == State::RUNNING;
                   }) >= NUM_PLAYERS) {
        // too many players
        *connection_ << false;
        connection_->flush();
        state = State::DONE;
        return;
      } else {
        *connection_ << true;
        connection_->flush();
        state = State::RUNNING;
      }
    }

    *connection_ << server_.map_.getSeed();
    connection_->flush();

    // TODO: rest of game logic
  } catch (SocketException const &e) {
    errorMessage = static_cast<string>(e);
    state = State::ERROR;
    return;
  } catch (StopFlag const &) {
    return;
  }
}

Server::Server(u32string const &password) noexcept
    : state(State::STARTING),
      errorMessage(),
      password_([&password]() {
        wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
        return converter.to_bytes(password);
      }()),
      stop_(false),
      server_(),
      rng_(random_device()()),
      map_(),
      thread_([this]() { return run(); }) {}

Server::~Server() {
  stop_ = true;
  thread_.join();
}

void Server::run() noexcept {
  try {
    map_.generate(rng_());

    server_ =
        networking::Server::makeServer(networking::PORT, password_, stop_);
    state = State::RUNNING;

    while (true) {
      unique_ptr<networking::Connection> accepted = server_->accept();
      if (accepted) {
        scoped_lock lock(connectionMutex_);
        connections_.emplace_back(*this, move(accepted));
      }

      {
        // reap dead connections
        scoped_lock lock(connectionMutex_);
        connections_.remove_if([](Connection const &c) {
          return c.state == Connection::State::DONE;
        });
      }
    }
  } catch (SocketException const &e) {
    errorMessage = static_cast<string>(e);
    state = State::ERROR;
    return;
  } catch (StopFlag const &) {
    return;
  }
}

unique_ptr<Server> server;
}  // namespace airewar::game
