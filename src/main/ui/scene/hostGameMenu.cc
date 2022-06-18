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

#include "ui/scene/hostGameMenu.h"

#include "ui/components.h"
#include "ui/scene/hostWaitingRoom.h"
#include "ui/scene/mainMenu.h"
#include "ui/window.h"

using namespace std;

namespace airewar::ui::scene {
class HostGameMenu final {
 public:
  HostGameMenu() noexcept
      : background(resources->hostGameBackground),
        title(Image2D::centered(resources->hostGameTitle, 0.5f, layout(0, 4))),
        passwordLabel(
            Image2D::alignBottom(resources->passwordLabel, 0.5f, layout(1, 4))),
        password(Textbox2D::alignTop(resources->orbitron, resources->textbox,
                                     {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f,
                                     layout(1, 4))),
        back(Button2D::alignRight(resources->backOn, resources->backOff, 0.5f,
                                  layout(3, 4))),
        host(Button2D::alignLeft(resources->hostOn, resources->hostOff, 0.5f,
                                 layout(3, 4))),
        clickedButton(nullptr),
        clickedTextbox(nullptr),
        activeTextbox(nullptr) {}
  HostGameMenu(HostGameMenu const &) noexcept = delete;
  HostGameMenu(HostGameMenu &&) noexcept = delete;

  ~HostGameMenu() noexcept = default;

  HostGameMenu &operator=(HostGameMenu const &) noexcept = delete;
  HostGameMenu &operator=(HostGameMenu &&) noexcept = delete;

  void draw() noexcept {
    background.draw();
    title.draw();
    passwordLabel.draw();
    password.draw();
    back.draw();
    host.draw();
  }

  void downAt(int32_t x, int32_t y) noexcept {
    if (activeTextbox) {
      SDL_StopTextInput();
      activeTextbox->active = false;
      activeTextbox = nullptr;
    }
    if (password.clicked(x, y)) {
      clickedTextbox = &password;
    } else if (back.clicked(x, y)) {
      clickedButton = &back;
      back.on = true;
    } else if (host.clicked(x, y)) {
      clickedButton = &host;
      host.on = true;
    }
  }

  enum class Action {
    NONE,
    BACK,
    HOST,
  };

  Action upAt(int32_t x, int32_t y) noexcept {
    if (clickedTextbox) {
      if (password.clicked(x, y) && clickedTextbox == &password) {
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
      } else if (host.clicked(x, y) && clickedButton == &host) {
        clickedButton = nullptr;
        return Action::HOST;
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

  u32string getPassword() const noexcept { return password; }

 private:
  Background2D background;
  Image2D title;
  Image2D passwordLabel;
  Textbox2D password;
  Button2D back;
  Button2D host;
  Button2D *clickedButton;
  Textbox2D *clickedTextbox;
  Textbox2D *activeTextbox;
};

void hostGameMenu() noexcept {
  HostGameMenu hostGameMenu;
  wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT: {
          SDL_PushEvent(&event);
          return;
        }
        case SDL_MOUSEBUTTONDOWN: {
          hostGameMenu.downAt(event.button.x, event.button.y);
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          switch (hostGameMenu.upAt(event.button.x, event.button.y)) {
            case HostGameMenu::Action::BACK: {
              return mainMenu();
            }
            case HostGameMenu::Action::HOST: {
              return hostWaitingRoom(hostGameMenu.getPassword());
            }
          }
          break;
        }
        case SDL_TEXTINPUT: {
          hostGameMenu.textInput(converter.from_bytes(event.text.text));
          break;
        }
        case SDL_TEXTEDITING: {
          hostGameMenu.textEditing(converter.from_bytes(event.edit.text));
          break;
        }
        case SDL_KEYDOWN: {
          switch (event.key.keysym.sym) {
            case SDLK_LEFT: {
              hostGameMenu.left();
              break;
            }
            case SDLK_RIGHT: {
              hostGameMenu.right();
              break;
            }
            case SDLK_HOME: {
              hostGameMenu.home();
              break;
            }
            case SDLK_END: {
              hostGameMenu.end();
              break;
            }
            case SDLK_BACKSPACE: {
              hostGameMenu.backspace();
              break;
            }
          }
          break;
        }
      }
    }

    hostGameMenu.draw();
    window->render();
  }
}
}  // namespace airewar::ui::scene
