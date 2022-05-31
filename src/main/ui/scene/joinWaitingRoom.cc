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

#include "ui/scene/joinWaitingRoom.h"

#include <memory>

#include "game/client.h"
#include "ui/components.h"
#include "ui/scene/joinGameMenu.h"
#include "ui/window.h"

using namespace std;
using namespace airewar::game;

namespace airewar::ui::scene {
class JoinWaitingRoom final {
 public:
  JoinWaitingRoom() noexcept
      : waiting_(resources->connectingBackground),
        error_(resources->errorBackground),
        errorMessage_(TextField2D::centered(
            resources->orbitron, resources->errorTextField,
            {1.0f, 0.0f, 0.0f, 1.0f}, 0.5f, layout(2, 4))),
        back_(Button2D::centered(resources->backOn, resources->backOff, 0.5f,
                                 layout(3, 4))),
        clicked_(nullptr),
        converter_() {}
  JoinWaitingRoom(JoinWaitingRoom const &) noexcept = delete;
  JoinWaitingRoom(JoinWaitingRoom &&) noexcept = delete;

  ~JoinWaitingRoom() noexcept = default;

  JoinWaitingRoom &operator=(JoinWaitingRoom const &) noexcept = delete;
  JoinWaitingRoom &operator=(JoinWaitingRoom &&) noexcept = delete;

  void draw() noexcept {
    // switch (client->state) {
    //   case Client::State::STARTING: {
    //     waiting_.draw();
    //     break;
    //   }
    //   case Client::State::ERROR: {
    //     error_.draw();
    //     errorMessage_.text = converter_.from_bytes(client->errorMessage);
    //     errorMessage_.draw();
    //     break;
    //   }
    // }
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

void joinWaitingRoom(u32string address, u32string password) noexcept {
  JoinWaitingRoom joinWaitingRoom;
  client = make_unique<Client>();
  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT: {
          client.reset();
          return;
        }
        case SDL_MOUSEBUTTONDOWN: {
          joinWaitingRoom.downAt(event.button.x, event.button.y);
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          switch (joinWaitingRoom.upAt(event.button.x, event.button.y)) {
            case JoinWaitingRoom::Action::BACK: {
              client.reset();
              return joinGameMenu();
            }
          }
          break;
        }
      }
    }

    // if (client->state == Client::State::RUNNING) {
    //   // TODO
    //   return;
    // }

    joinWaitingRoom.draw();
    window->render();
  }
}
}  // namespace airewar::ui::scene
