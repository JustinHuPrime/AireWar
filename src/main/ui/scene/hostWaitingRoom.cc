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
      : waiting_(resources->connectingBackground),
        error_(resources->errorBackground),
        errorMessage_(TextField2D::centered(
            resources->orbitron, resources->errorTextField,
            {1.0f, 0.0f, 0.0f, 1.0f}, 0.5f, layout(2, 4))),
        back_(Button2D::centered(resources->backOn, resources->backOff, 0.5f,
                                 layout(3, 4))),
        clicked_(nullptr),
        converter_() {}
  HostWaitingRoom(HostWaitingRoom const &) noexcept = delete;
  HostWaitingRoom(HostWaitingRoom &&) noexcept = delete;

  ~HostWaitingRoom() noexcept = default;

  HostWaitingRoom &operator=(HostWaitingRoom const &) noexcept = delete;
  HostWaitingRoom &operator=(HostWaitingRoom &&) noexcept = delete;

  void draw() noexcept {
    switch (server->state) {
      case Server::State::STARTING: {
        waiting_.draw();
        break;
      }
      case Server::State::ERROR: {
        error_.draw();
        errorMessage_.text = converter_.from_bytes(server->errorMessage);
        errorMessage_.draw();
        break;
      }
      case Server::State::RUNNING: {
        if (client) {
          switch (client->state) {
            case Client::State::STARTING: {
              waiting_.draw();
              break;
            }
            case Client::State::ERROR: {
              error_.draw();
              errorMessage_.text = converter_.from_bytes(client->errorMessage);
              errorMessage_.draw();
              break;
            }
          }
          break;
        }
      }
    }
    back_.draw();
  }

  void downAt(int32_t x, int32_t y) noexcept {
    if (back_.clicked(x, y)) {
      clicked_ = &back_;
      back_.on = true;
    }
  }

  enum class Action {
    NONE,
    BACK,
  };

  Action upAt(int32_t x, int32_t y) noexcept {
    if (clicked_) {
      clicked_->on = false;
      if (back_.clicked(x, y) && clicked_ == &back_) {
        clicked_ = nullptr;
        return Action::BACK;
      } else {
        clicked_ = nullptr;
        return Action::NONE;
      }
    } else {
      return Action::NONE;
    }
  }

 private:
  Background2D waiting_;
  Background2D error_;
  TextField2D errorMessage_;
  Button2D back_;
  Button2D *clicked_;
  wstring_convert<codecvt_utf8<char32_t>, char32_t> converter_;
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
