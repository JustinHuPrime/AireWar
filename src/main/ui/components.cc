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

#include "ui/components.h"

#include <vector>

#include "window.h"

using namespace airewar::util;
using namespace std;
using namespace glm;

namespace airewar::ui {
namespace {
float clipX(float x) { return x * 2.0f - 1.0f; }

float clipY(float y) { return (1.0f - y) * 2.0f - 1.0f; }

float scaleX(Texture2D const &tex) {
  return tex.getWidth() / Texture2D::SCREEN_WIDTH;
}

float scaleY(Texture2D const &tex) {
  return tex.getWidth() / Texture2D::SCREEN_HEIGHT;
}

float tex2Window(float x) {
  return x / Texture2D::SCREEN_WIDTH * window->getWidth();
}

float distance(float x1, float y1, float x2, float y2) {
  return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void drawChar(Font &font, VBO &glyphVBO, float &x, float y,
              char32_t c) noexcept {
  Glyph &glyph = font.glyph(c);
  glyphVBO.update({clipX(x + glyph.xMin / window->getWidth()),
                   clipY(y - glyph.yMin / window->getHeight()), 0.0f, 1.0f,
                   clipX(x + glyph.xMax / window->getWidth()),
                   clipY(y - glyph.yMin / window->getHeight()), 1.0f, 1.0f,
                   clipX(x + glyph.xMax / window->getWidth()),
                   clipY(y - glyph.yMax / window->getHeight()), 1.0f, 0.0f,
                   clipX(x + glyph.xMin / window->getWidth()),
                   clipY(y - glyph.yMax / window->getHeight()), 0.0f, 0.0f},
                  0);
  glyph.texture.use(GL_TEXTURE0);
  resources->text2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  x += glyph.advance / window->getWidth();
}
}  // namespace

Background2D::Background2D(Texture2D &texture) noexcept : texture(texture) {}

void Background2D::draw() noexcept {
  texture.use(GL_TEXTURE0);
  ScopeGuard guard = resources->backgroundVAO.use();
  resources->image2D.use();
  resources->image2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

Image2D Image2D::centered(Texture2D &texture, float x, float y) noexcept {
  return Image2D(texture, x - scaleX(texture) / 2.0f,
                 y - scaleY(texture) / 2.0f);
}
Image2D Image2D::alignBottom(Texture2D &texture, float x, float y) noexcept {
  return Image2D(texture, x - scaleX(texture) / 2.0f, y - scaleY(texture));
}
Image2D::Image2D(Texture2D &texture, float x, float y) noexcept
    : texture(texture),
      vbo(
          {
              clipX(x), clipY(y + scaleY(texture)),  // bottom left pos
              0.0f, 0.0f,                            // bottom left tex

              clipX(x + scaleX(texture)),
              clipY(y + scaleY(texture)),  // bottom right pos
              1.0f, 0.0f,                  // bottom right tex

              clipX(x + scaleX(texture)), clipY(y),  // top right pos
              1.0f, 1.0f,                            // top right tex

              clipX(x), clipY(y),  // top left pos
              0.0f, 1.0f,          // top left tex
          },
          GL_STATIC_DRAW),
      vao(vbo, resources->quadEBO, resources->quadAttributes) {}

void Image2D::draw() noexcept {
  texture.use(GL_TEXTURE0);
  ScopeGuard guard = vao.use();
  resources->image2D.use();
  resources->image2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

Button2D Button2D::centered(Texture2D &onTexture, Texture2D &offTexture,
                            float x, float y) noexcept {
  return Button2D(onTexture, offTexture, x - scaleX(onTexture) / 2.0f,
                  y - scaleY(onTexture) / 2.0f);
}

Button2D Button2D::alignRight(Texture2D &onTexture, Texture2D &offTexture,
                              float x, float y) noexcept {
  return Button2D(onTexture, offTexture, x - scaleX(onTexture),
                  y - scaleY(onTexture) / 2.0f);
}
Button2D Button2D::alignLeft(Texture2D &onTexture, Texture2D &offTexture,
                             float x, float y) noexcept {
  return Button2D(onTexture, offTexture, x, y - scaleY(onTexture) / 2.0f);
}

Button2D::Button2D(Texture2D &onTexture, Texture2D &offTexture, float x,
                   float y) noexcept
    : on(false),
      onTexture(onTexture),
      offTexture(offTexture),
      vbo(
          {
              clipX(x), clipY(y + scaleY(onTexture)),  // bottom left pos
              0.0f, 0.0f,                              // bottom left tex

              clipX(x + scaleX(onTexture)),
              clipY(y + scaleY(onTexture)),  // bottom right pos
              1.0f, 0.0f,                    // bottom right tex

              clipX(x + scaleX(onTexture)), clipY(y),  // top right pos
              1.0f, 1.0f,                              // top right tex

              clipX(x), clipY(y),  // top left pos
              0.0f, 1.0f,          // top left tex
          },
          GL_STATIC_DRAW),
      vao(vbo, resources->quadEBO, resources->quadAttributes),
      left(x * window->getWidth()),
      right((x + scaleX(onTexture)) * window->getWidth()),
      top(y * window->getHeight()),
      bottom((y + scaleY(onTexture)) * window->getHeight()) {
  assert((onTexture.getWidth() == offTexture.getWidth()) &&
         "on and off textures must have the same width");
  assert((onTexture.getHeight() == offTexture.getHeight()) &&
         "on and off textures must have the same height");
}

void Button2D::draw() noexcept {
  (on ? onTexture : offTexture).use(GL_TEXTURE0);
  ScopeGuard guard = vao.use();
  resources->image2D.use();
  resources->image2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

bool Button2D::clicked(int32_t x, int32_t y) const noexcept {
  float scaledRadius = tex2Window(RADIUS);
  float innerLeft = left + scaledRadius;
  float innerRight = right - scaledRadius;
  float innerTop = top + scaledRadius;
  float innerBottom = bottom - scaledRadius;
  if (innerLeft <= x && x <= innerRight && top <= y && y <= bottom)
    return true;
  else if (left <= x && x <= right && innerTop <= y && y <= innerBottom)
    return true;
  else if (distance(innerLeft, innerBottom, x, y) <= scaledRadius ||
           distance(innerLeft, innerTop, x, y) <= scaledRadius ||
           distance(innerRight, innerTop, x, y) <= scaledRadius ||
           distance(innerRight, innerBottom, x, y) <= scaledRadius)
    return true;
  else
    return false;
}

Textbox2D Textbox2D::alignTop(Font &font, Texture2D &texture,
                              vec4 const &colour, float x, float y) noexcept {
  return Textbox2D(font, texture, colour, x - scaleX(texture) / 2.0f, y);
}

Textbox2D::Textbox2D(Font &font, Texture2D &texture, vec4 const &colour,
                     float x, float y) noexcept
    : active(false),
      font(font),
      texture(texture),
      colour(colour),
      vbo(
          {
              clipX(x), clipY(y + scaleY(texture)),  // bottom left pos
              0.0f, 0.0f,                            // bottom left tex

              clipX(x + scaleX(texture)),
              clipY(y + scaleY(texture)),  // bottom right pos
              1.0f, 0.0f,                  // bottom right tex

              clipX(x + scaleX(texture)), clipY(y),  // top right pos
              1.0f, 1.0f,                            // top right tex

              clipX(x), clipY(y),  // top left pos
              0.0f, 1.0f,          // top left tex
          },
          GL_STATIC_DRAW),
      vao(vbo, resources->quadEBO, resources->quadAttributes),
      left(x * window->getWidth()),
      right((x + scaleX(texture)) * window->getWidth()),
      top(y * window->getHeight()),
      bottom((y + scaleY(texture)) * window->getHeight()),
      preCursor(),
      composition(),
      postCursor(),
      glyphVBO(vector<float>(16), GL_DYNAMIC_DRAW),
      glyphVAO(glyphVBO, resources->quadEBO, resources->quadAttributes),
      cursorVBO(vector<float>(4), GL_DYNAMIC_DRAW),
      cursorVAO(cursorVBO, resources->cursorEBO, resources->cursorAttributes) {}

Textbox2D::operator std::u32string() const noexcept {
  return preCursor + composition + postCursor;
}

void Textbox2D::draw() noexcept {
  texture.use(GL_TEXTURE0);
  ScopeGuard guard = vao.use();
  resources->image2D.use();
  resources->image2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  font.setSize(bottom - top - 2.0f * tex2Window(RADIUS));

  Glyph const &bar = font.glyph(U'|');
  float baseline =
      (bottom - tex2Window(RADIUS) + bar.yMin) / window->getHeight();
  float left = (left + tex2Window(RADIUS)) / window->getWidth();

  glyphVAO.use(guard);
  resources->text2D.use();
  resources->text2D.setUniform("colour", colour);
  for (char32_t const &c : preCursor)
    drawChar(font, glyphVBO, left, baseline, c);
  for (char32_t const &c : composition)
    drawChar(font, glyphVBO, left, baseline, c);
  float cursorPos = left;
  for (char32_t const &c : postCursor)
    drawChar(font, glyphVBO, left, baseline, c);
  if (active) {
    cursorVAO.use(guard);
    cursorVBO.update(
        {clipX(cursorPos),
         clipY((bottom - tex2Window(RADIUS)) / window->getHeight()),
         clipX(cursorPos),
         clipY((top + tex2Window(RADIUS)) / window->getHeight())},
        0);
    resources->solid2D.use();
    resources->solid2D.setUniform("colour", {0.0f, 0.0f, 0.0f, 1.0f});
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, nullptr);
  }

  // TODO: cursor blink
}

bool Textbox2D::clicked(int32_t x, int32_t y) const noexcept {
  float scaledRadius = tex2Window(RADIUS);
  float innerLeft = left + scaledRadius;
  float innerRight = right - scaledRadius;
  float innerTop = top + scaledRadius;
  float innerBottom = bottom - scaledRadius;
  if (innerLeft <= x && x <= innerRight && top <= y && y <= bottom)
    return true;
  else if (left <= x && x <= right && innerTop <= y && y <= innerBottom)
    return true;
  else if (distance(innerLeft, innerBottom, x, y) <= scaledRadius ||
           distance(innerLeft, innerTop, x, y) <= scaledRadius ||
           distance(innerRight, innerTop, x, y) <= scaledRadius ||
           distance(innerRight, innerBottom, x, y) <= scaledRadius)
    return true;
  else
    return false;
}

void Textbox2D::textEditing(std::u32string const &text) noexcept {
  composition = text;
}

void Textbox2D::textInput(std::u32string const &text) noexcept {
  preCursor += text;
}

void Textbox2D::cursorLeft() noexcept {
  postCursor = composition + postCursor;
  composition.clear();
  if (!preCursor.empty()) {
    char32_t moved = preCursor.back();
    preCursor.pop_back();
    postCursor = moved + postCursor;
  }
}

void Textbox2D::cursorRight() noexcept {
  preCursor += composition;
  composition.clear();
  if (!postCursor.empty()) {
    char32_t moved = postCursor.front();
    postCursor.erase(0, 1);
    preCursor += moved;
  }
}

void Textbox2D::cursorHome() noexcept {
  postCursor = preCursor + composition + postCursor;
  composition.clear();
  preCursor.clear();
}

void Textbox2D::cursorEnd() noexcept {
  preCursor += composition + postCursor;
  composition.clear();
  postCursor.clear();
}

void Textbox2D::backspace() noexcept {
  if (!composition.empty()) {
    composition.pop_back();
  } else if (!preCursor.empty()) {
    preCursor.pop_back();
  }
}

TextField2D TextField2D::centered(Font &font, Texture2D &texture,
                                  vec4 const &colour, float x,
                                  float y) noexcept {
  return TextField2D(font, texture, colour, x - scaleX(texture) / 2.0f,
                     y - scaleY(texture) / 2.0f);
}

TextField2D::TextField2D(Font &font_, Texture2D &texture_, vec4 const &colour_,
                         float x, float y) noexcept
    : text(),
      font(font),
      texture(texture),
      colour(colour),
      vbo(
          {
              clipX(x), clipY(y + scaleY(texture)),  // bottom left pos
              0.0f, 0.0f,                            // bottom left tex

              clipX(x + scaleX(texture)),
              clipY(y + scaleY(texture)),  // bottom right pos
              1.0f, 0.0f,                  // bottom right tex

              clipX(x + scaleX(texture)), clipY(y),  // top right pos
              1.0f, 1.0f,                            // top right tex

              clipX(x), clipY(y),  // top left pos
              0.0f, 1.0f,          // top left tex
          },
          GL_STATIC_DRAW),
      vao(vbo, resources->quadEBO, resources->quadAttributes),
      left(x * window->getWidth()),
      right((x + scaleX(texture)) * window->getWidth()),
      top(y * window->getHeight()),
      bottom((y + scaleY(texture)) * window->getHeight()),
      glyphVBO(vector<float>(16), GL_DYNAMIC_DRAW),
      glyphVAO(glyphVBO, resources->quadEBO, resources->quadAttributes) {}

void TextField2D::draw() noexcept {
  texture.use(GL_TEXTURE0);
  ScopeGuard guard = vao.use();
  resources->image2D.use();
  resources->image2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  font.setSize(bottom - top - 2.0f * tex2Window(RADIUS));

  Glyph const &bar = font.glyph(U'|');
  float baseline =
      (bottom - tex2Window(RADIUS) + bar.yMin) / window->getHeight();
  float left = (left + tex2Window(RADIUS)) / window->getWidth();

  glyphVAO.use(guard);
  resources->text2D.use();
  resources->text2D.setUniform("colour", colour);
  for (char32_t const &c : text) drawChar(font, glyphVBO, left, baseline, c);
}

float layout(size_t index, size_t count) noexcept {
  return (index + 0.5f) / count;
}
}  // namespace airewar::ui
