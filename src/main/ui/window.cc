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

#include "ui/window.h"

#include <GL/glew.h>

#include <iostream>

#include "options.h"
#include "ui/resources.h"
#include "util/exceptions/initException.h"

using namespace airewar::util::exceptions;
using namespace std;

namespace airewar::ui {
namespace {
void setAttribute(SDL_GLattr attr, int value) {
  if (SDL_GL_SetAttribute(attr, value) != 0)
    throw InitException("Could not set OpenGL attribute", SDL_GetError());
}

#ifndef NDEBUG
void debugMessageCallback(GLenum source, GLenum type, GLuint id,
                          GLenum severity, GLsizei, GLchar const *message,
                          void const *) {
  cerr << "DEBUG: OpenGL Debug Message" << endl;
  switch (source) {
    case GL_DEBUG_SOURCE_API: {
      cerr << "source = API" << endl;
      break;
    }
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
      cerr << "source = window system" << endl;
      break;
    }
    case GL_DEBUG_SOURCE_SHADER_COMPILER: {
      cerr << "source = shader compiler" << endl;
      break;
    }
    case GL_DEBUG_SOURCE_THIRD_PARTY: {
      cerr << "source = third party" << endl;
      break;
    }
    case GL_DEBUG_SOURCE_APPLICATION: {
      cerr << "source = application" << endl;
      break;
    }
    case GL_DEBUG_SOURCE_OTHER: {
      cerr << "source = other" << endl;
      break;
    }
  }
  switch (type) {
    case GL_DEBUG_TYPE_ERROR: {
      cerr << "type = error" << endl;
      break;
    }
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
      cerr << "type = deprecated behavior" << endl;
      break;
    }
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
      cerr << "type = undefined behavior" << endl;
      break;
    }
    case GL_DEBUG_TYPE_PORTABILITY: {
      cerr << "type = portability" << endl;
      break;
    }
    case GL_DEBUG_TYPE_PERFORMANCE: {
      cerr << "type = performance" << endl;
      break;
    }
    case GL_DEBUG_TYPE_MARKER: {
      cerr << "type = marker" << endl;
      break;
    }
    case GL_DEBUG_TYPE_PUSH_GROUP: {
      cerr << "type = push group" << endl;
      break;
    }
    case GL_DEBUG_TYPE_POP_GROUP: {
      cerr << "type = pop group" << endl;
      break;
    }
    case GL_DEBUG_TYPE_OTHER: {
      cerr << "type = other" << endl;
      break;
    }
  }
  cerr << "id = " << id << endl;
  switch (severity) {
    case GL_DEBUG_SEVERITY_LOW: {
      cerr << "severity = low" << endl;
      break;
    }
    case GL_DEBUG_SEVERITY_MEDIUM: {
      cerr << "severity = medium" << endl;
      break;
    }
    case GL_DEBUG_SEVERITY_HIGH: {
      cerr << "severity = high" << endl;
      break;
    }
    case GL_DEBUG_SEVERITY_NOTIFICATION: {
      cerr << "severity = notification" << endl;
      break;
    }
  }
  cerr << "message = " << message << endl;

  // if (type == GL_DEBUG_TYPE_ERROR) abort();
}
#endif
}  // namespace

Window::Window()
    : window(nullptr, SDL_DestroyWindow),
      context(nullptr, SDL_GL_DeleteContext) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    throw InitException("Could not initialize SDL", SDL_GetError());

  window.reset(SDL_CreateWindow(
      "AireWar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0,
      SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL));
  if (!window) throw InitException("Could not create window", SDL_GetError());

  if (options->msaa != Options::MSAALevel::ZERO) {
    setAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    setAttribute(SDL_GL_MULTISAMPLESAMPLES, static_cast<int>(options->msaa));
  }

  setAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  setAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#ifndef NDEBUG
  setAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
  setAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  context.reset(SDL_GL_CreateContext(window.get()));

  if (GLenum status = glewInit(); status != GLEW_OK)
    throw InitException(
        "Could not initialize GLEW",
        reinterpret_cast<char const *>(glewGetErrorString(status)));

  if (options->vsync) {
    if (SDL_GL_SetSwapInterval(-1) != 0)
      if (SDL_GL_SetSwapInterval(1) != 0)
        throw InitException("Could not enable vsync", SDL_GetError());
  } else {
    if (SDL_GL_SetSwapInterval(0) != 0)
      throw InitException("Could not disable vsync", SDL_GetError());
  }

  SDL_GL_GetDrawableSize(window.get(), &width, &height);
  if (width > height * 16 / 9) {
    // overly wide
    int padding = (width - height * 16 / 9) / 2;
    width = height * 16 / 9;
    glViewport(padding, 0, width, height);
  } else if (height > width * 9 / 16) {
    // overly tall
    int padding = (height - width * 9 / 16) / 2;
    height = width * 9 / 16;
    glViewport(0, padding, width, height);
  } else {
    // exactly 16:9
    glViewport(0, 0, width, height);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifndef NDEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(debugMessageCallback, nullptr);
#endif

  clear();
}

Window::~Window() noexcept {
  resources.reset();  // avoid static deinit order fiasco
  SDL_Quit();
}

void Window::render() noexcept { SDL_GL_SwapWindow(window.get()); }

SDL_Window *Window::getWindow() noexcept { return window.get(); }
int Window::getWidth() const noexcept { return width; }
int Window::getHeight() const noexcept { return height; }

void Window::clear() noexcept {
  glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

unique_ptr<Window> window;
}  // namespace airewar::ui
