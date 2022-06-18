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

#include "ui/scene/joinGameMenu.h"

#include <codecvt>
#include <locale>

#include "options.h"
#include "ui/components.h"
#include "ui/scene/joinWaitingRoom.h"
#include "ui/scene/mainMenu.h"
#include "ui/window.h"

using namespace std;

namespace airewar::ui::scene {
class JoinGameMenu final {
 public:
  JoinGameMenu() noexcept
      : background(resources->joinGameBackground),
        title(Image2D::centered(resources->joinGameTitle, 0.5f, layout(0, 4))),
        serverAddressLabel(Image2D::alignBottom(resources->serverAddressLabel,
                                                0.5f, layout(1, 4))),
        serverAddress(
            Textbox2D::alignTop(resources->orbitron, resources->textbox,
                                {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, layout(1, 4))),
        passwordLabel(
            Image2D::alignBottom(resources->passwordLabel, 0.5f, layout(2, 4))),
        password(Textbox2D::alignTop(resources->orbitron, resources->textbox,
                                     {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f,
                                     layout(2, 4))),
        back(Button2D::alignRight(resources->backOn, resources->backOff, 0.5f,
                                  layout(3, 4))),
        join(Button2D::alignLeft(resources->joinOn, resources->joinOff, 0.5f,
                                 layout(3, 4))),
        clickedButton(nullptr),
        clickedTextbox(nullptr),
        activeTextbox(nullptr) {}

  JoinGameMenu(JoinGameMenu const &) noexcept = delete;
  JoinGameMenu(JoinGameMenu &&) noexcept = delete;

  ~JoinGameMenu() noexcept = default;

  JoinGameMenu &operator=(JoinGameMenu const &) noexcept = delete;
  JoinGameMenu &operator=(JoinGameMenu &&) noexcept = delete;

  void draw() noexcept {
    background.draw();
    title.draw();
    serverAddressLabel.draw();
    serverAddress.draw();
    passwordLabel.draw();
    password.draw();
    back.draw();
    join.draw();
  }

  void downAt(int32_t x, int32_t y) noexcept {
    if (activeTextbox) {
      SDL_StopTextInput();
      activeTextbox->active = false;
      activeTextbox = nullptr;
    }
    if (serverAddress.clicked(x, y)) {
      clickedTextbox = &serverAddress;
    } else if (password.clicked(x, y)) {
      clickedTextbox = &password;
    } else if (back.clicked(x, y)) {
      clickedButton = &back;
      back.on = true;
    } else if (join.clicked(x, y)) {
      clickedButton = &join;
      join.on = true;
    }
  }

  enum class Action {
    NONE,
    BACK,
    JOIN,
  };

  Action upAt(int32_t x, int32_t y) noexcept {
    if (clickedTextbox) {
      if (serverAddress.clicked(x, y) && clickedTextbox == &serverAddress) {
        activeTextbox = clickedTextbox;
        SDL_StartTextInput();
        serverAddress.active = true;
      } else if (password.clicked(x, y) && clickedTextbox == &password) {
        activeTextbox = clickedTextbox;
        SDL_StartTextInput();
        password.active = true;
      }
      clickedTextbox = nullptr;
      return Action::NONE;
    } else if (clickedButton) {
      clickedButton->on = false;
      if (back.clicked(x, y) && clickedButton == &back) {
        clickedButton = nullptr;
        return Action::BACK;
      } else if (join.clicked(x, y) && clickedButton == &join) {
        clickedButton = nullptr;
        return Action::JOIN;
      } else {
        clickedButton = nullptr;
        return Action::NONE;
      }
    } else {
      return Action::NONE;
    }
  }

  void textInput(u32string const &text) noexcept {
    if (activeTextbox) activeTextbox->textInput(text);
  }

  void textEditing(u32string const &text) noexcept {
    if (activeTextbox) activeTextbox->textEditing(text);
  }

  void left() noexcept {
    if (activeTextbox) activeTextbox->cursorLeft();
  }

  void right() noexcept {
    if (activeTextbox) activeTextbox->cursorRight();
  }

  void home() noexcept {
    if (activeTextbox) activeTextbox->cursorHome();
  }

  void end() noexcept {
    if (activeTextbox) activeTextbox->cursorEnd();
  }

  void backspace() noexcept {
    if (activeTextbox) activeTextbox->backspace();
  }

  u32string getAddress() const noexcept { return serverAddress; }

  u32string getPassword() const noexcept { return password; }

 private:
  Background2D background;
  Image2D title;
  Image2D serverAddressLabel;
  Textbox2D serverAddress;
  Image2D passwordLabel;
  Textbox2D password;
  Button2D back;
  Button2D join;
  Button2D *clickedButton;
  Textbox2D *clickedTextbox;
  Textbox2D *activeTextbox;
};

void joinGameMenu() noexcept {
  JoinGameMenu joinGameMenu;
  wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT: {
          return;
        }
        case SDL_MOUSEBUTTONDOWN: {
          joinGameMenu.downAt(event.button.x, event.button.y);
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          switch (joinGameMenu.upAt(event.button.x, event.button.y)) {
            case JoinGameMenu::Action::BACK: {
              return mainMenu();
            }
            case JoinGameMenu::Action::JOIN: {
              return joinWaitingRoom(joinGameMenu.getAddress(),
                                     joinGameMenu.getPassword());
            }
          }
          break;
        }
        case SDL_TEXTINPUT: {
          joinGameMenu.textInput(converter.from_bytes(event.text.text));
          break;
        }
        case SDL_TEXTEDITING: {
          joinGameMenu.textEditing(converter.from_bytes(event.edit.text));
          break;
        }
        case SDL_KEYDOWN: {
          switch (event.key.keysym.sym) {
            case SDLK_LEFT: {
              joinGameMenu.left();
              break;
            }
            case SDLK_RIGHT: {
              joinGameMenu.right();
              break;
            }
            case SDLK_HOME: {
              joinGameMenu.home();
              break;
            }
            case SDLK_END: {
              joinGameMenu.end();
              break;
            }
            case SDLK_BACKSPACE: {
              joinGameMenu.backspace();
              break;
            }
          }
          break;
        }
      }
    }

    joinGameMenu.draw();
    window->render();
  }
}
}  // namespace airewar::ui::scene
