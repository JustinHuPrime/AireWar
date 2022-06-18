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
      : background(resources->mainMenuBackground),
        title(Image2D::centered(resources->mainMenuTitle, 0.5f, layout(0, 5))),
        joinGame(Button2D::centered(
            resources->joinGameOn, resources->joinGameOff, 0.5f, layout(1, 5))),
        hostGame(Button2D::centered(
            resources->hostGameOn, resources->hostGameOff, 0.5f, layout(2, 5))),
        options(Button2D::centered(resources->optionsOn, resources->optionsOff,
                                    0.5, layout(3, 5))),
        quit(Button2D::centered(resources->quitOn, resources->quitOff, 0.5,
                                 layout(4, 5))),
        clicked(nullptr) {}
  MainMenu(MainMenu const &) noexcept = delete;
  MainMenu(MainMenu &&) noexcept = delete;

  ~MainMenu() noexcept = default;

  MainMenu &operator=(MainMenu const &) noexcept = delete;
  MainMenu &operator=(MainMenu &&) noexcept = delete;

  void draw() noexcept {
    background.draw();
    title.draw();
    joinGame.draw();
    hostGame.draw();
    options.draw();
    quit.draw();
  }

  void downAt(int32_t x, int32_t y) noexcept {
    if (joinGame.clicked(x, y)) {
      clicked = &joinGame;
      joinGame.on = true;
    } else if (hostGame.clicked(x, y)) {
      clicked = &hostGame;
      hostGame.on = true;
    } else if (options.clicked(x, y)) {
      clicked = &options;
      options.on = true;
    } else if (quit.clicked(x, y)) {
      clicked = &quit;
      quit.on = true;
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
    if (clicked) {
      clicked->on = false;
      if (joinGame.clicked(x, y) && clicked == &joinGame) {
        clicked = nullptr;
        return Action::JOIN;
      } else if (hostGame.clicked(x, y) && clicked == &hostGame) {
        clicked = nullptr;
        return Action::HOST;
      } else if (options.clicked(x, y) && clicked == &options) {
        clicked = nullptr;
        return Action::OPTIONS;
      } else if (quit.clicked(x, y) && clicked == &quit) {
        clicked = nullptr;
        return Action::QUIT;
      } else {
        clicked = nullptr;
        return Action::NONE;
      }
    } else {
      return Action::NONE;
    }
  }

 private:
  Background2D background;
  Image2D title;
  Button2D joinGame;
  Button2D hostGame;
  Button2D options;
  Button2D quit;
  Button2D *clicked;
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
