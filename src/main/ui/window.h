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

#ifndef AIREWAR_UI_WINDOW_H_
#define AIREWAR_UI_WINDOW_H_

#include <SDL.h>

#include <memory>

namespace airewar::ui {
class Window final {
 public:
  Window();
  Window(Window const &) noexcept = delete;
  Window(Window &&) noexcept = delete;

  ~Window() noexcept;

  Window &operator=(Window const &) noexcept = delete;
  Window &operator=(Window &&) noexcept = delete;

  void render() noexcept;

  SDL_Window *getWindow() noexcept;
  int getWidth() const noexcept;
  int getHeight() const noexcept;

  void clear() noexcept;

 private:
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
  std::unique_ptr<std::remove_pointer<SDL_GLContext>::type,
                  decltype(&SDL_GL_DeleteContext)>
      context;
  int width;
  int height;
};

extern std::unique_ptr<Window> window;
}  // namespace airewar::ui

#endif  // AIREWAR_UI_WINDOW_H_
