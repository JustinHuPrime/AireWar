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

#include "ui/scene/hostWaitingRoom.h"

#include <memory>

#include "game/client.h"
#include "game/server.h"
#include "ui/components.h"
#include "ui/scene/hostGameMenu.h"
#include "ui/window.h"

using namespace std;
using namespace airewar::game;

namespace airewar::ui::scene {
class HostWaitingRoom final {
 public:
  HostWaitingRoom() noexcept
      : waiting(resources->connectingBackground),
        error(resources->errorBackground),
        errorMessage(TextField2D::centered(
            resources->orbitron, resources->errorTextField,
            {1.0f, 0.0f, 0.0f, 1.0f}, 0.5f, layout(2, 4))),
        generating(resources->generatingMapBackground),
        back(Button2D::centered(resources->backOn, resources->backOff, 0.5f,
                                layout(3, 4))),
        clicked(nullptr),
        converter() {}
  HostWaitingRoom(HostWaitingRoom const &) noexcept = delete;
  HostWaitingRoom(HostWaitingRoom &&) noexcept = delete;

  ~HostWaitingRoom() noexcept = default;

  HostWaitingRoom &operator=(HostWaitingRoom const &) noexcept = delete;
  HostWaitingRoom &operator=(HostWaitingRoom &&) noexcept = delete;

  void draw() noexcept {
    switch (server->state) {
      case Server::State::STARTING: {
        waiting.draw();
        break;
      }
      case Server::State::ERROR: {
        error.draw();
        errorMessage.text = converter.from_bytes(server->errorMessage);
        errorMessage.draw();
        break;
      }
      case Server::State::RUNNING: {
        if (client) {
          switch (client->state) {
            case Client::State::STARTING: {
              waiting.draw();
              break;
            }
            case Client::State::ERROR: {
              error.draw();
              errorMessage.text = converter.from_bytes(client->errorMessage);
              errorMessage.draw();
              break;
            }
            case Client::State::GENERATING_MAP: {
              generating.draw();
              break;
            }
          }
          break;
        }
      }
    }
    back.draw();
  }

  void downAt(int32_t x, int32_t y) noexcept {
    if (back.clicked(x, y)) {
      clicked = &back;
      back.on = true;
    }
  }

  enum class Action {
    NONE,
    BACK,
  };

  Action upAt(int32_t x, int32_t y) noexcept {
    if (clicked) {
      clicked->on = false;
      if (back.clicked(x, y) && clicked == &back) {
        clicked = nullptr;
        return Action::BACK;
      } else {
        clicked = nullptr;
        return Action::NONE;
      }
    } else {
      return Action::NONE;
    }
  }

 private:
  Background2D waiting;
  Background2D error;
  TextField2D errorMessage;
  Background2D generating;
  Button2D back;
  Button2D *clicked;
  wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
};

void hostWaitingRoom(u32string password) noexcept {
  HostWaitingRoom hostWaitingRoom;
  server = make_unique<Server>(password);

  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT: {
          SDL_PushEvent(&event);
          client.reset();
          server.reset();
          return;
        }
        case SDL_MOUSEBUTTONDOWN: {
          hostWaitingRoom.downAt(event.button.x, event.button.y);
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          switch (hostWaitingRoom.upAt(event.button.x, event.button.y)) {
            case HostWaitingRoom::Action::BACK: {
              client.reset();
              server.reset();
              return hostGameMenu();
            }
          }
          break;
        }
      }
    }

    if (server->state == Server::State::RUNNING && !client) {
      client = make_unique<Client>(U"localhost", password);
    } else if (client && client->state == Client::State::RUNNING) {
      // TODO
      return;
    }

    hostWaitingRoom.draw();
    window->render();
  }
}
}  // namespace airewar::ui::scene
