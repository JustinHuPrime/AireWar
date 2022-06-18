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

#ifndef AIREWAR_UI_RESOURCES_H_
#define AIREWAR_UI_RESOURCES_H_

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "glm/glm.hpp"
#include "ui/freetype.h"
#include "util/pairHash.h"
#include "util/scopeGuard.h"

namespace airewar::ui {
class GLResource {
 public:
  GLResource(GLResource const &) noexcept = delete;
  GLResource(GLResource &&) noexcept;

  virtual ~GLResource() noexcept = default;

  GLResource &operator=(GLResource const &) noexcept = delete;
  GLResource &operator=(GLResource &&) noexcept;

  unsigned get() noexcept;

 protected:
  unsigned id;

  GLResource() noexcept;
  explicit GLResource(unsigned id) noexcept;
};

class Shader : public GLResource {
 public:
  Shader() noexcept = default;
  Shader(GLenum type, std::filesystem::path const &filename);
  Shader(Shader const &) noexcept = delete;
  Shader(Shader &&) noexcept = default;

  ~Shader() noexcept override;

  Shader &operator=(Shader const &) noexcept = delete;
  Shader &operator=(Shader &&) noexcept = default;
};

class VertexShader final : public Shader {
 public:
  explicit VertexShader(std::filesystem::path const &filename);
  VertexShader(VertexShader const &) noexcept = delete;
  VertexShader(VertexShader &&) noexcept = default;

  ~VertexShader() noexcept override = default;

  VertexShader &operator=(VertexShader const &) noexcept = delete;
  VertexShader &operator=(VertexShader &&) noexcept = default;
};

class FragmentShader final : public Shader {
 public:
  explicit FragmentShader(std::filesystem::path const &filename);
  FragmentShader(FragmentShader const &) noexcept = delete;
  FragmentShader(FragmentShader &&) noexcept = default;

  ~FragmentShader() noexcept override = default;

  FragmentShader &operator=(FragmentShader const &) noexcept = delete;
  FragmentShader &operator=(FragmentShader &&) noexcept = default;

 private:
};

class ShaderProgram final : public GLResource {
 public:
  ShaderProgram() noexcept = default;
  ShaderProgram(VertexShader &, FragmentShader &) noexcept;
  ShaderProgram(ShaderProgram const &) noexcept = delete;
  ShaderProgram(ShaderProgram &&) noexcept = default;

  ~ShaderProgram() noexcept;

  ShaderProgram &operator=(ShaderProgram const &) noexcept = delete;
  ShaderProgram &operator=(ShaderProgram &&) noexcept = default;

  void use() noexcept;

  ShaderProgram &setUniform(std::string const &name, int value) noexcept;
  ShaderProgram &setUniform(std::string const &name,
                            glm::vec4 const &value) noexcept;
  ShaderProgram &setUniform(std::string const &name,
                            glm::mat4 const &value) noexcept;

 private:
  std::unordered_map<std::string, int> uniforms;

  int getUniformLocation(std::string const &name) noexcept;
};

struct Glyph;
class Texture2D final : public GLResource {
  friend struct Glyph;

 public:
  Texture2D() noexcept = default;
  explicit Texture2D(std::filesystem::path const &filename);
  Texture2D(Texture2D const &) noexcept = delete;
  Texture2D(Texture2D &&) noexcept = default;

  ~Texture2D() noexcept;

  Texture2D &operator=(Texture2D const &) noexcept = delete;
  Texture2D &operator=(Texture2D &&) noexcept = default;

  void use(GLenum textureNumber) noexcept;

  int getWidth() const noexcept;
  int getHeight() const noexcept;

  static constexpr float SCREEN_WIDTH = 3840.0f;
  static constexpr float SCREEN_HEIGHT = 2160.0f;

 private:
  Texture2D(int width, int height, void const *pixels) noexcept;

  int width;
  int height;
};

class VBO final : public GLResource {
 public:
  VBO() noexcept = default;
  VBO(std::vector<float> const &data, GLenum usage) noexcept;
  VBO(VBO const &) noexcept = delete;
  VBO(VBO &&) noexcept = default;

  ~VBO() noexcept;

  VBO &operator=(VBO const &) noexcept = delete;
  VBO &operator=(VBO &&) noexcept = default;

  void use() noexcept;

  void update(std::vector<float> const &data, size_t offset) noexcept;
};

class EBO final : public GLResource {
 public:
  EBO() noexcept = default;
  EBO(std::vector<unsigned> const &data, GLenum usage) noexcept;
  EBO(EBO const &) noexcept = delete;
  EBO(EBO &&) noexcept = default;

  ~EBO() noexcept;

  EBO &operator=(EBO const &) noexcept = delete;
  EBO &operator=(EBO &&) noexcept = default;

  void use() noexcept;
};

class VAO final : public GLResource {
 public:
  class Attribute final {
   public:
    int size;
    GLenum type;
    bool normalized;
    int stride;
    int offset;

    Attribute(Attribute const &) noexcept = default;
    Attribute(Attribute &&) noexcept = default;

    ~Attribute() noexcept = default;

    Attribute &operator=(Attribute const &) noexcept = default;
    Attribute &operator=(Attribute &&) noexcept = default;

    static Attribute floats(int size, int stride, int offset) noexcept;

   private:
    Attribute(int size, GLenum type, bool normalized, int stride,
              int offset) noexcept;
  };

  VAO() noexcept = default;
  VAO(VBO &, EBO &, std::vector<Attribute> const &) noexcept;
  VAO(VAO const &) noexcept = delete;
  VAO(VAO &&) noexcept = default;

  ~VAO() noexcept;

  VAO &operator=(VAO const &) noexcept = delete;
  VAO &operator=(VAO &&) noexcept = default;

  airewar::util::ScopeGuard use() noexcept;
  void use(airewar::util::ScopeGuard &previous) noexcept;
};

struct Glyph final {
  explicit Glyph(FT_GlyphSlot glyph) noexcept;
  Glyph(Glyph const &) noexcept = delete;
  Glyph(Glyph &&) noexcept = default;

  ~Glyph() noexcept = default;

  Glyph &operator=(Glyph const &) noexcept = delete;
  Glyph &operator=(Glyph &&) noexcept = default;

  Texture2D texture;
  float xMin;
  float xMax;
  float yMin;
  float yMax;
  float advance;
};

class Font final {
 public:
  Font() noexcept;
  explicit Font(std::filesystem::path const &filename);
  Font(Font const &) noexcept = delete;
  Font(Font &&) noexcept = default;

  ~Font() noexcept = default;

  Font &operator=(Font const &) noexcept = delete;
  Font &operator=(Font &&) noexcept = default;

  Font &setSize(unsigned size) noexcept;
  Glyph &glyph(char32_t c) const noexcept;

 private:
  std::unique_ptr<std::remove_pointer<FT_Face>::type, decltype(&FT_Done_Face)>
      face;
  unsigned size;
  std::unordered_map<std::pair<char32_t, unsigned>, Glyph,
                     airewar::util::hash<unsigned, char32_t>> mutable cache;
};

class ResourceManager final {
 public:
  // splash screen
  ShaderProgram image2D;

  Texture2D splash;

  VBO backgroundVBO;
  EBO quadEBO;
  std::vector<VAO::Attribute> quadAttributes;
  VAO backgroundVAO;
  std::unique_ptr<SDL_Cursor, decltype(&SDL_FreeCursor)> busyCursor;

  // generic menu
  Font orbitron;
  Texture2D backOn;
  Texture2D backOff;
  Texture2D textbox;
  Texture2D passwordLabel;
  ShaderProgram text2D;
  ShaderProgram solid2D;
  EBO cursorEBO;
  std::vector<VAO::Attribute> cursorAttributes;

  // main menu
  Texture2D mainMenuBackground;
  Texture2D mainMenuTitle;
  Texture2D joinGameOn;
  Texture2D joinGameOff;
  Texture2D hostGameOn;
  Texture2D hostGameOff;
  Texture2D optionsOn;
  Texture2D optionsOff;
  Texture2D quitOn;
  Texture2D quitOff;

  // join game
  Texture2D joinGameBackground;
  Texture2D joinGameTitle;
  Texture2D serverAddressLabel;
  Texture2D joinOn;
  Texture2D joinOff;

  // host game
  Texture2D hostGameBackground;
  Texture2D hostGameTitle;
  Texture2D hostOn;
  Texture2D hostOff;

  // options
  Texture2D optionsBackground;
  Texture2D optionsTitle;

  // waiting room
  Texture2D connectingBackground;
  Texture2D errorBackground;
  Texture2D errorTextField;
  Texture2D generatingMapBackground;

  std::unique_ptr<SDL_Cursor, decltype(&SDL_FreeCursor)> arrowCursor;

  ResourceManager() noexcept;
  ResourceManager(ResourceManager const &) noexcept = delete;
  ResourceManager(ResourceManager &&) noexcept = delete;

  ~ResourceManager() noexcept = default;

  ResourceManager &operator=(ResourceManager const &) noexcept = delete;
  ResourceManager &operator=(ResourceManager &&) noexcept = delete;

  void loadSplash();
  void loadGame();

 private:
  // save between loads
  std::unique_ptr<VertexShader> image2Dv;
};

extern std::unique_ptr<ResourceManager> resources;
}  // namespace airewar::ui

#endif  // AIREWAR_UI_RESOURCES_H_
