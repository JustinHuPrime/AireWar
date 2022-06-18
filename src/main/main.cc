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

#include <SDL2/SDL.h>
#include <sodium.h>
#include <stb_image.h>

#include <iostream>
#include <sstream>

#include "options.h"
#include "ui/components.h"
#include "ui/freetype.h"
#include "ui/resources.h"
#include "ui/scene/mainMenu.h"
#include "ui/window.h"
#include "util/exceptions/initException.h"
#include "version.h"

#if defined(__linux__)
#include <signal.h>
#elif
#error "operating system not supported/recognized"
#endif

using namespace std;
using namespace airewar;
using namespace airewar::util::exceptions;
using namespace airewar::ui;
using namespace airewar::ui::scene;

int main(int argc, char *argv[]) {
  cout << "AireWar version " << VERSION_MAJOR << "." << VERSION_MINOR << "."
       << VERSION_PATCH << endl;
  cout << "Copyright 2022 Justin Hu" << endl;
  cout << "This is free software; see the source for copying conditions. There "
          "is NO"
       << endl;
  cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR "
          "PURPOSE."
       << endl;

  if (argc >= 2) {
    cerr << "AireWar is a graphical program and does not accept command line "
            "options!"
         << endl;
    return EXIT_FAILURE;
  }

  try {
    // initialize and check sodium
    if (sodium_init() == -1)
      throw InitException("Could not initialize libsodium");
    if (SODIUM_LIBRARY_VERSION_MAJOR != sodium_library_version_major() ||
        SODIUM_LIBRARY_VERSION_MINOR > sodium_library_version_minor()) {
      stringstream ss;
      ss << "Expected " << SODIUM_VERSION_STRING << " or later, but found "
         << sodium_version_string();
      throw InitException("Incompatible libsodium version", ss.str());
    }

    // don't initalize, but check sdl2
    SDL_version linked;
    SDL_GetVersion(&linked);
    SDL_version compiled;
    SDL_VERSION(&compiled);
    if (compiled.major != linked.major || compiled.minor > linked.minor) {
      stringstream ss;
      ss << "Expected SDL version " << compiled.major << "." << compiled.minor
         << "." << compiled.patch << " or later, but found " << linked.major
         << "." << linked.minor << "." << linked.patch;
      throw InitException("Incompatible SDL version", ss.str());
    }

    // initialize and check freetype
    freetype = make_unique<FreeType>();
    int linkedMajor, linkedMinor, linkedPatch;
    FT_Library_Version(freetype->get(), &linkedMajor, &linkedMinor,
                       &linkedPatch);
    if (FREETYPE_MAJOR != linkedMajor || FREETYPE_MINOR > linkedMinor) {
      stringstream ss;
      ss << "Expected FreeType version " << FREETYPE_MAJOR << "."
         << FREETYPE_MINOR << "." << FREETYPE_PATCH << " or later, but found "
         << linkedMajor << "." << linkedMinor << "." << linkedPatch;
      throw InitException("Incompatible FreeType version", ss.str());
    }

    // set up stbi
    stbi_set_flip_vertically_on_load(true);

    // os-specific setup
#if defined(__linux__)
    // turn off SIGPIPE
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &act, nullptr);
#elif
#error "operating system not supported/recognized"
#endif

    // setup static objects
    options = make_unique<Options>();
    window = make_unique<Window>();
    resources = make_unique<ResourceManager>();

    // load resources
    resources->loadSplash();
    SDL_SetCursor(resources->busyCursor.get());
    Background2D splash(resources->splash);
    splash.draw();
    window->render();
    resources->loadGame();
    SDL_SetCursor(resources->arrowCursor.get());

    // start actual game
    mainMenu();

    return EXIT_SUCCESS;
  } catch (InitException const &e) {
    if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, e.getTitle().c_str(),
                                 e.getMessage().c_str(),
                                 window ? window->getWindow() : nullptr) != 0) {
      cerr << "ERROR: " << e.getTitle() << ": " << e.getMessage() << endl;
      return EXIT_FAILURE;
    }
  }
}
