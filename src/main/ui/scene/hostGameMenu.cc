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
      : background_(resources->hostGameBackground),
        title_(Image2D::centered(resources->hostGameTitle, 0.5f, layout(0, 4))),
        passwordLabel_(
            Image2D::alignBottom(resources->passwordLabel, 0.5f, layout(1, 4))),
        password_(Textbox2D::alignTop(resources->orbitron, resources->textbox,
                                      {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f,
                                      layout(1, 4))),
        back_(Button2D::alignRight(resources->backOn, resources->backOff, 0.5f,
                                   layout(3, 4))),
        host_(Button2D::alignLeft(resources->hostOn, resources->hostOff, 0.5f,
                                  layout(3, 4))),
        clickedButton_(nullptr),
        clickedTextbox_(nullptr),
        activeTextbox_(nullptr) {}
  HostGameMenu(HostGameMenu const &) noexcept = delete;
  HostGameMenu(HostGameMenu &&) noexcept = delete;

  ~HostGameMenu() noexcept = default;

  HostGameMenu &operator=(HostGameMenu const &) noexcept = delete;
  HostGameMenu &operator=(HostGameMenu &&) noexcept = delete;

  void draw() noexcept {
    background_.draw();
    title_.draw();
    passwordLabel_.draw();
    password_.draw();
    back_.draw();
    host_.draw();
  }

  void downAt(int32_t x, int32_t y) noexcept {
    if (activeTextbox_) {
      SDL_StopTextInput();
      activeTextbox_->active = false;
      activeTextbox_ = nullptr;
    }
    if (password_.clicked(x, y)) {
      clickedTextbox_ = &password_;
    } else if (back_.clicked(x, y)) {
      clickedButton_ = &back_;
      back_.on = true;
    } else if (host_.clicked(x, y)) {
      clickedButton_ = &host_;
      host_.on = true;
    }
  }

  enum class Action {
    NONE,
    BACK,
    HOST,
  };

  Action upAt(int32_t x, int32_t y) noexcept {
    if (clickedTextbox_) {
      if (password_.clicked(x, y) && clickedTextbox_ == &password_) {
        activeTextbox_ = clickedTextbox_;
        SDL_StartTextInput();
        password_.active = true;
      }
      clickedTextbox_ = nullptr;
      return Action::NONE;
    } else if (clickedButton_) {
      clickedButton_->on = false;
      if (back_.clicked(x, y) && clickedButton_ == &back_) {
        clickedButton_ = nullptr;
        return Action::BACK;
      } else if (host_.clicked(x, y) && clickedButton_ == &host_) {
        clickedButton_ = nullptr;
        return Action::HOST;
      } else {
        clickedButton_ = nullptr;
        return Action::NONE;
      }
    } else {
      return Action::NONE;
    }
  }

  void textInput(u32string const &text) noexcept {
    if (activeTextbox_) activeTextbox_->textInput(text);
  }

  void textEditing(u32string const &text) noexcept {
    if (activeTextbox_) activeTextbox_->textEditing(text);
  }

  void left() noexcept {
    if (activeTextbox_) activeTextbox_->left();
  }

  void right() noexcept {
    if (activeTextbox_) activeTextbox_->right();
  }

  void home() noexcept {
    if (activeTextbox_) activeTextbox_->home();
  }

  void end() noexcept {
    if (activeTextbox_) activeTextbox_->end();
  }

  void backspace() noexcept {
    if (activeTextbox_) activeTextbox_->backspace();
  }

  u32string password() const noexcept { return password_; }

 private:
  Background2D background_;
  Image2D title_;
  Image2D passwordLabel_;
  Textbox2D password_;
  Button2D back_;
  Button2D host_;
  Button2D *clickedButton_;
  Textbox2D *clickedTextbox_;
  Textbox2D *activeTextbox_;
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
              return hostWaitingRoom(hostGameMenu.password());
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
