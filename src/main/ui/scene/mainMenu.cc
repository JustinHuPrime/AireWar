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

#include "ui/scene/mainMenu.h"

#include <SDL.h>

#include "ui/components.h"
#include "ui/scene/hostGameMenu.h"
#include "ui/scene/joinGameMenu.h"
#include "ui/scene/optionsMenu.h"
#include "ui/window.h"

namespace airewar::ui::scene {
class MainMenu final {
 public:
  MainMenu() noexcept
      : background_(resources->mainMenuBackground),
        title_(Image2D::centered(resources->mainMenuTitle, 0.5f, layout(0, 5))),
        joinGame_(Button2D::centered(
            resources->joinGameOn, resources->joinGameOff, 0.5f, layout(1, 5))),
        hostGame_(Button2D::centered(
            resources->hostGameOn, resources->hostGameOff, 0.5f, layout(2, 5))),
        options_(Button2D::centered(resources->optionsOn, resources->optionsOff,
                                    0.5, layout(3, 5))),
        quit_(Button2D::centered(resources->quitOn, resources->quitOff, 0.5,
                                 layout(4, 5))),
        clicked_(nullptr) {}
  MainMenu(MainMenu const &) noexcept = delete;
  MainMenu(MainMenu &&) noexcept = delete;

  ~MainMenu() noexcept = default;

  MainMenu &operator=(MainMenu const &) noexcept = delete;
  MainMenu &operator=(MainMenu &&) noexcept = delete;

  void draw() noexcept {
    background_.draw();
    title_.draw();
    joinGame_.draw();
    hostGame_.draw();
    options_.draw();
    quit_.draw();
  }

  void downAt(int32_t x, int32_t y) noexcept {
    if (joinGame_.clicked(x, y)) {
      clicked_ = &joinGame_;
      joinGame_.on = true;
    } else if (hostGame_.clicked(x, y)) {
      clicked_ = &hostGame_;
      hostGame_.on = true;
    } else if (options_.clicked(x, y)) {
      clicked_ = &options_;
      options_.on = true;
    } else if (quit_.clicked(x, y)) {
      clicked_ = &quit_;
      quit_.on = true;
    }
  }

  enum class Action {
    NONE,
    JOIN,
    HOST,
    OPTIONS,
    QUIT,
  };

  Action upAt(int32_t x, int32_t y) noexcept {
    if (clicked_) {
      clicked_->on = false;
      if (joinGame_.clicked(x, y) && clicked_ == &joinGame_) {
        clicked_ = nullptr;
        return Action::JOIN;
      } else if (hostGame_.clicked(x, y) && clicked_ == &hostGame_) {
        clicked_ = nullptr;
        return Action::HOST;
      } else if (options_.clicked(x, y) && clicked_ == &options_) {
        clicked_ = nullptr;
        return Action::OPTIONS;
      } else if (quit_.clicked(x, y) && clicked_ == &quit_) {
        clicked_ = nullptr;
        return Action::QUIT;
      } else {
        clicked_ = nullptr;
        return Action::NONE;
      }
    } else {
      return Action::NONE;
    }
  }

 private:
  Background2D background_;
  Image2D title_;
  Button2D joinGame_;
  Button2D hostGame_;
  Button2D options_;
  Button2D quit_;
  Button2D *clicked_;
};

void mainMenu() noexcept {
  MainMenu mainMenu;

  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT: {
          return;
        }
        case SDL_MOUSEBUTTONDOWN: {
          mainMenu.downAt(event.button.x, event.button.y);
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          switch (mainMenu.upAt(event.button.x, event.button.y)) {
            case MainMenu::Action::JOIN: {
              return joinGameMenu();
            }
            case MainMenu::Action::HOST: {
              return hostGameMenu();
            }
            case MainMenu::Action::OPTIONS: {
              return optionsMenu();
            }
            case MainMenu::Action::QUIT: {
              return;
            }
          }
          break;
        }
      }
    }

    mainMenu.draw();
    window->render();
  }
}
}  // namespace airewar::ui::scene
