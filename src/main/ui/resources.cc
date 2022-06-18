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

#include "ui/resources.h"

#include <stb_image.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "glm/gtc/type_ptr.hpp"
#include "util/exceptions/initException.h"

using namespace std;
using namespace std::filesystem;
using namespace glm;
using namespace airewar::util::exceptions;
using namespace airewar::util;

namespace airewar::ui {
GLResource::GLResource() noexcept : id(0) {}

GLResource::GLResource(unsigned id) noexcept : id(id) {}

GLResource::GLResource(GLResource &&other) noexcept : id(other.id) {
  other.id = 0;
}

GLResource &GLResource::operator=(GLResource &&other) noexcept {
  swap(id, other.id);
  return *this;
}

unsigned GLResource::get() noexcept { return id; }

Shader::Shader(GLenum type, path const &filename)
    : GLResource(glCreateShader(type)) {
  assert((type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER) &&
         "type must be a GL_VERTEX_SHADER or GL_FRAGMENT_SHADER");

  path p(ASSET_PREFIX);
  p /= "shaders";
  p /= filename;

  try {
    ifstream fin;
    fin.exceptions(ios_base::badbit | ios_base::failbit);
    fin.open(p, ios_base::in);
    stringstream ss;
    ss << fin.rdbuf();
    string code = ss.str();
    char const *codePtr = code.c_str();
    glShaderSource(id, 1, &codePtr, nullptr);
  } catch (ios_base::failure const &e) {
    throw InitException("Failed to load shader " + filename.string(),
                        "Could not read file " + filename.string());
  }

  glCompileShader(id);

#ifndef NDEBUG
  int status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    std::unique_ptr<char[]> log = make_unique<char[]>(length + 1);
    glGetShaderInfoLog(id, length, nullptr, log.get());
    log[length] = '\0';
    cerr << "ERROR: Failed to compile " << filename << endl;
    cerr << log.get() << endl;
  }
#endif
}

Shader::~Shader() noexcept {
  if (id != 0) glDeleteShader(id);
}

VertexShader::VertexShader(path const &filename)
    : Shader(GL_VERTEX_SHADER, filename) {}

FragmentShader::FragmentShader(path const &filename)
    : Shader(GL_FRAGMENT_SHADER, filename) {}

ShaderProgram::ShaderProgram(VertexShader &vs, FragmentShader &fs) noexcept
    : GLResource(glCreateProgram()), uniforms() {
  glAttachShader(id, vs.get());
  glAttachShader(id, fs.get());
  glLinkProgram(id);

#ifndef NDEBUG
  int status;
  glGetProgramiv(id, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    int length;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    std::unique_ptr<char[]> log = make_unique<char[]>(length + 1);
    glGetProgramInfoLog(id, length, nullptr, log.get());
    log[length] = '\0';
    cerr << "ERROR: Failed to link shaders" << endl;
    cerr << log.get() << endl;
  }
#endif

  glDetachShader(id, vs.get());
  glDetachShader(id, fs.get());
}

ShaderProgram::~ShaderProgram() noexcept {
  if (id != 0) glDeleteProgram(id);
}

void ShaderProgram::use() noexcept { glUseProgram(id); }

ShaderProgram &ShaderProgram::setUniform(std::string const &name,
                                         int value) noexcept {
  assert([this]() {
    unsigned currId;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<int *>(&currId));
    return currId == id;
  }() && "active shader isn't the shader whose uniforms are being set");

  glUniform1i(getUniformLocation(name), value);

  return *this;
}

ShaderProgram &ShaderProgram::setUniform(std::string const &name,
                                         vec4 const &value) noexcept {
  assert([this]() {
    unsigned currId;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<int *>(&currId));
    return currId == id;
  }() && "active shader isn't the shader whose uniforms are being set");

  glUniform4fv(getUniformLocation(name), 1, value_ptr(value));

  return *this;
}

ShaderProgram &ShaderProgram::setUniform(std::string const &name,
                                         mat4 const &value) noexcept {
  assert([this]() {
    unsigned currId;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<int *>(&currId));
    return currId == id;
  }() && "active shader isn't the shader whose uniforms are being set");

  glUniformMatrix4fv(getUniformLocation(name), 1, false, value_ptr(value));

  return *this;
}

int ShaderProgram::getUniformLocation(std::string const &name) noexcept {
  unordered_map<string, int>::iterator found = uniforms.find(name);
  if (found != uniforms.end()) {
    return found->second;
  } else {
    assert((glGetUniformLocation(id, name.c_str()) != -1) &&
           "name doesn't exist as a uniform in the shader program");
    return uniforms[name] = glGetUniformLocation(id, name.c_str());
  }
}

Texture2D::Texture2D(path const &filename) {
  glCreateTextures(GL_TEXTURE_2D, 1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  path p(ASSET_PREFIX);
  p /= "textures";
  p /= filename;

  unique_ptr<uint8_t, void (*)(void *)> data(
      stbi_load(p.c_str(), &width, &height, nullptr, 4), stbi_image_free);
  if (!data)
    throw InitException("Failed to load texture " + filename.string(),
                        "Could not read file " + filename.string());

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data.get());

  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture2D::Texture2D(int width, int height, void const *pixels) noexcept
    : width(width), height(height) {
  glCreateTextures(GL_TEXTURE_2D, 1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
               GL_UNSIGNED_BYTE, pixels);

  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture2D::~Texture2D() noexcept {
  if (id != 0) glDeleteTextures(1, &id);
}

void Texture2D::use(GLenum textureNumber) noexcept {
  glActiveTexture(textureNumber);
  glBindTexture(GL_TEXTURE_2D, id);
}

int Texture2D::getWidth() const noexcept { return width; }

int Texture2D::getHeight() const noexcept { return height; }

VBO::VBO(vector<float> const &data, GLenum usage) noexcept
    : GLResource([]() {
        unsigned id;
        glGenBuffers(1, &id);
        return id;
      }()) {
  use();
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(),
               usage);
}

VBO::~VBO() noexcept {
  if (id != 0) glDeleteBuffers(1, &id);
}

void VBO::use() noexcept { glBindBuffer(GL_ARRAY_BUFFER, id); }

void VBO::update(std::vector<float> const &data, size_t offset) noexcept {
  use();
  glBufferSubData(GL_ARRAY_BUFFER, offset, data.size() * sizeof(float),
                  data.data());
}

EBO::EBO(vector<unsigned> const &data, GLenum usage) noexcept
    : GLResource([]() {
        unsigned id;
        glGenBuffers(1, &id);
        return id;
      }()) {
  use();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(unsigned),
               data.data(), usage);
}

EBO::~EBO() noexcept {
  if (id != 0) glDeleteBuffers(1, &id);
}

void EBO::use() noexcept { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }

VAO::VAO(VBO &vbo, EBO &ebo, vector<VAO::Attribute> const &attributes) noexcept
    : GLResource([]() {
        unsigned id;
        glGenVertexArrays(1, &id);
        return id;
      }()) {
  ScopeGuard guard = use();
  vbo.use();
  ebo.use();

  for (unsigned idx = 0; idx < attributes.size(); ++idx) {
    glVertexAttribPointer(
        idx, attributes[idx].size, attributes[idx].type,
        attributes[idx].normalized, attributes[idx].stride,
        reinterpret_cast<void const *>(attributes[idx].offset));
    glEnableVertexAttribArray(idx);
  }
}

VAO::~VAO() noexcept {
  if (id != 0) glDeleteVertexArrays(1, &id);
}

VAO::Attribute VAO::Attribute::floats(int size, int stride,
                                      int offset) noexcept {
  assert((1 <= size && size <= 4) &&
         "vertex attribute size must be in the range [1, 4]");
  return VAO::Attribute(size, GL_FLOAT, false,
                        stride * static_cast<int>(sizeof(float)),
                        offset * static_cast<int>(sizeof(float)));
}

VAO::Attribute::Attribute(int size, GLenum type, bool normalized, int stride,
                          int offset) noexcept
    : size(size),
      type(type),
      normalized(normalized),
      stride(stride),
      offset(offset) {}

ScopeGuard VAO::use() noexcept {
  glBindVertexArray(id);
  return ScopeGuard([]() { glBindVertexArray(0); });
}

void VAO::use(ScopeGuard &previous) noexcept {
  previous.reset([]() { glBindVertexArray(0); });
  glBindVertexArray(id);
}

Glyph::Glyph(FT_GlyphSlot glyph) noexcept
    : texture(glyph->bitmap.width, glyph->bitmap.rows, glyph->bitmap.buffer),
      xMin(glyph->bitmap_left),
      xMax(static_cast<float>(glyph->bitmap_left) + glyph->bitmap.width),
      yMin(static_cast<float>(glyph->bitmap_top) - glyph->bitmap.rows),
      yMax(glyph->bitmap_top),
      advance(glyph->advance.x / 64) {}

Font::Font() noexcept : face(nullptr, FT_Done_Face) {}

Font::Font(path const &filename)
    : face(
          [&filename]() {
            FT_Face face;
            path p(ASSET_PREFIX);
            p /= filename;

            if (FT_New_Face(freetype->get(), p.c_str(), 0, &face) != FT_Err_Ok)
              throw InitException("Failed to load font " + filename.string(),
                                  "Could not read file " + filename.string());

            return face;
          }(),
          FT_Done_Face) {}

Font &Font::setSize(unsigned size) noexcept {
  size = size;
  FT_Set_Pixel_Sizes(face.get(), 0, size);
  return *this;
}

Glyph &Font::glyph(char32_t c) const noexcept {
  pair<unsigned, char32_t> key(size, c);
  auto found = cache.find(key);
  if (found != cache.end()) {
    return found->second;
  } else {
#ifndef NDEBUG
    FT_Error result =
#endif
        FT_Load_Glyph(face.get(), FT_Get_Char_Index(face.get(), c),
                      FT_LOAD_RENDER);
    assert((result == FT_Err_Ok) && "Failed to load glyph");
    cache.emplace(key, Glyph(face.get()->glyph));
    return cache.at(key);
  }
}

ResourceManager::ResourceManager() noexcept
    : busyCursor(nullptr, SDL_FreeCursor),
      arrowCursor(nullptr, SDL_FreeCursor) {}

void ResourceManager::loadSplash() {
  splash = Texture2D("splash.tga");
  backgroundVBO = VBO(
      {
          // position        // tex coord
          -1.0f, -1.0f, /**/ 0.0f, 0.0f,  // bottom left
          1.0f, -1.0f, /**/ 1.0f, 0.0f,   // bottom right
          1.0f, 1.0f, /**/ 1.0f, 1.0f,    // top right
          -1.0f, 1.0f, /**/ 0.0f, 1.0f,   // top left
      },
      GL_STATIC_DRAW);
  quadEBO = EBO({0, 1, 2, 0, 2, 3}, GL_STATIC_DRAW);
  quadAttributes = {
      VAO::Attribute::floats(2, 4, 0),
      VAO::Attribute::floats(2, 4, 2),
  };
  backgroundVAO = VAO(backgroundVBO, quadEBO, quadAttributes);
  busyCursor.reset(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT));
  image2Dv = make_unique<VertexShader>("image2D.v.glsl");
  FragmentShader image2Df("image2D.f.glsl");
  image2D = ShaderProgram(*image2Dv, image2Df);
}

void ResourceManager::loadGame() {
  orbitron = Font("orbitron.ttf");
  backOn = Texture2D(path("menu") / "backOn.tga");
  backOff = Texture2D(path("menu") / "backOff.tga");
  textbox = Texture2D(path("menu") / "textbox.tga");
  passwordLabel = Texture2D(path("menu") / "passwordLabel.tga");
  FragmentShader text2Df("text2D.f.glsl");
  text2D = ShaderProgram(*image2Dv, text2Df);
  VertexShader solid2Dv("solid2D.v.glsl");
  FragmentShader solid2Df("solid2D.f.glsl");
  solid2D = ShaderProgram(solid2Dv, solid2Df);
  cursorEBO = EBO({0, 1}, GL_STATIC_DRAW);
  cursorAttributes = {VAO::Attribute::floats(2, 2, 0)};

  mainMenuBackground = Texture2D(path("mainMenu") / "background.tga");
  mainMenuTitle = Texture2D(path("mainMenu") / "title.tga");
  joinGameOn = Texture2D(path("mainMenu") / "joinGameOn.tga");
  joinGameOff = Texture2D(path("mainMenu") / "joinGameOff.tga");
  hostGameOn = Texture2D(path("mainMenu") / "hostGameOn.tga");
  hostGameOff = Texture2D(path("mainMenu") / "hostGameOff.tga");
  optionsOn = Texture2D(path("mainMenu") / "optionsOn.tga");
  optionsOff = Texture2D(path("mainMenu") / "optionsOff.tga");
  quitOn = Texture2D(path("mainMenu") / "quitOn.tga");
  quitOff = Texture2D(path("mainMenu") / "quitOff.tga");

  joinGameBackground = Texture2D(path("joinMenu") / "background.tga");
  joinGameTitle = Texture2D(path("joinMenu") / "title.tga");
  serverAddressLabel = Texture2D(path("joinMenu") / "serverAddressLabel.tga");
  joinOn = Texture2D(path("joinMenu") / "joinOn.tga");
  joinOff = Texture2D(path("joinMenu") / "joinOff.tga");

  hostGameBackground = Texture2D(path("hostMenu") / "background.tga");
  hostGameTitle = Texture2D(path("hostMenu") / "title.tga");
  hostOn = Texture2D(path("hostMenu") / "hostOn.tga");
  hostOff = Texture2D(path("hostMenu") / "hostOff.tga");

  optionsBackground = Texture2D(path("optionsMenu") / "background.tga");
  optionsTitle = Texture2D(path("optionsMenu") / "title.tga");

  connectingBackground =
      Texture2D(path("waitingRoom") / "connectingBackground.tga");
  errorBackground = Texture2D(path("waitingRoom") / "errorBackground.tga");
  errorTextField = Texture2D(path("waitingRoom") / "errorTextField.tga");
  generatingMapBackground =
      Texture2D(path("waitingRoom") / "generatingMapBackground.tga");

  arrowCursor.reset(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));

  image2Dv.reset();
}

std::unique_ptr<ResourceManager> resources;
}  // namespace airewar::ui
