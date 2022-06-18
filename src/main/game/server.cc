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
      server(server),
      connection(move(socket)),
      thread([this]() { return run(); }) {}

Server::Connection::~Connection() { thread.join(); }

void Server::Connection::run() noexcept {
  try {
    if (!connection->handshake(server.password)) {
      // incorrect password - kill connection
      state = State::DONE;
      return;
    }

    {
      // check for number of players
      scoped_lock lock(server.connectionMutex);
      if (count_if(server.connections.begin(), server.connections.end(),
                   [](Connection const &connection) {
                     return connection.state == State::RUNNING;
                   }) >= NUM_PLAYERS) {
        // too many players
        *connection << false;
        connection->flush();
        state = State::DONE;
        return;
      } else {
        *connection << true;
        connection->flush();
        state = State::RUNNING;
      }
    }

    *connection << server.map.getSeed();
    connection->flush();

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
      password([&password]() {
        wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
        return converter.to_bytes(password);
      }()),
      stop(false),
      server(),
      rng(random_device()()),
      map(),
      thread([this]() { return run(); }) {}

Server::~Server() {
  stop = true;
  thread.join();
}

void Server::run() noexcept {
  try {
    map.generate(rng());

    server = networking::Server::makeServer(networking::PORT, password, stop);
    state = State::RUNNING;

    while (true) {
      unique_ptr<networking::Connection> accepted = server->accept();
      if (accepted) {
        scoped_lock lock(connectionMutex);
        connections.emplace_back(*this, move(accepted));
      }

      {
        // reap dead connections
        scoped_lock lock(connectionMutex);
        connections.remove_if([](Connection const &c) {
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
