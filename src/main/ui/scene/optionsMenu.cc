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

#include "ui/scene/optionsMenu.h"

#include "ui/components.h"
#include "ui/window.h"

namespace airewar::ui::scene {
class OptionsMenu final {
 public:
  OptionsMenu() noexcept
      : background(resources->optionsBackground),
        title(Image2D::centered(resources->optionsTitle, 0.5f, layout(0, 7))),
        cancel(Button2D::centered(resources->backOn, resources->backOff, 0.25f,
                                   layout(6, 7))),
        accept(Button2D::centered(resources->joinOn, resources->joinOff, 0.75f,
                                   layout(6, 7))) {
  }  // TODO: use appropriate buttons
  OptionsMenu(OptionsMenu const &) noexcept = delete;
  OptionsMenu(OptionsMenu &&) noexcept = delete;

  ~OptionsMenu() noexcept = default;

  OptionsMenu &operator=(OptionsMenu const &) noexcept = delete;
  OptionsMenu &operator=(OptionsMenu &&) noexcept = delete;

  void draw() noexcept {
    background.draw();
    title.draw();
    cancel.draw();
    accept.draw();
  }

  void downAt(int32_t x, int32_t y) noexcept {}

  enum class Action {
    NONE,
  };

  Action upAt(int32_t x, int32_t y) noexcept { return Action::NONE; }

 private:
  Background2D background;
  Image2D title;
  Button2D cancel;
  Button2D accept;
};

void optionsMenu() noexcept {
  OptionsMenu optionsMenu;
  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT: {
          return;
        }
        case SDL_MOUSEBUTTONDOWN: {
          optionsMenu.downAt(event.button.x, event.button.y);
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          switch (optionsMenu.upAt(event.button.x, event.button.y)) {}
          break;
        }
      }
    }

    optionsMenu.draw();
    window->render();
  }
}
}  // namespace airewar::ui::scene
