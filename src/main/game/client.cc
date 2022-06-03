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

#include "game/client.h"

#include <codecvt>
#include <locale>

#include "options.h"
#include "sodium.h"
#include "util/exceptions/socketException.h"
#include "util/exceptions/stopFlag.h"

using namespace std;
using namespace airewar::util::exceptions;
using namespace airewar::game::networking;

namespace airewar::game {
Client::Client(u32string const &address, u32string const &password) noexcept
    : map(),
      state(State::STARTING),
      errorMessage(),
      address_(address),
      password_(password),
      stop_(false),
      connection_(),
      thread_([this]() { return run(); }) {}

Client::~Client() noexcept {
  stop_ = true;
  thread_.join();
}

void Client::run() noexcept {
  try {
    map.initTriangles();

    wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
    string addressStr = converter.to_bytes(address_);
    connection_ = networking::Connection::makeClient(
        converter.to_bytes(address_), networking::PORT, stop_);

    if (!connection_->handshake(converter.to_bytes(password_))) {
      errorMessage = "Incorrect password";
      state = State::ERROR;
      return;
    }

    bool hasSlot;
    *connection_ >> hasSlot;
    if (!hasSlot) {
      errorMessage = "No slot available";
      state = State::ERROR;
      return;
    }

    uint64_t seed;
    *connection_ >> seed;
    map.setSeed(seed);
    state = State::GENERATING_MAP;

    // TODO: generate map
  } catch (SocketException const &e) {
    errorMessage = static_cast<string>(e);
    state = State::ERROR;
    return;
  } catch (StopFlag const &) {
    return;
  }
}

unique_ptr<Client> client;
}  // namespace airewar::game
