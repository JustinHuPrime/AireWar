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
  return tex.width() / Texture2D::SCREEN_WIDTH;
}

float scaleY(Texture2D const &tex) {
  return tex.height() / Texture2D::SCREEN_HEIGHT;
}

float tex2Window(float x) {
  return x / Texture2D::SCREEN_WIDTH * window->width();
}

float distance(float x1, float y1, float x2, float y2) {
  return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void drawChar(Font &font, VBO &glyphVBO, float &x, float y,
              char32_t c) noexcept {
  Glyph &glyph = font.glyph(c);
  glyphVBO.update({clipX(x + glyph.xMin / window->width()),
                   clipY(y - glyph.yMin / window->height()), 0.0f, 1.0f,
                   clipX(x + glyph.xMax / window->width()),
                   clipY(y - glyph.yMin / window->height()), 1.0f, 1.0f,
                   clipX(x + glyph.xMax / window->width()),
                   clipY(y - glyph.yMax / window->height()), 1.0f, 0.0f,
                   clipX(x + glyph.xMin / window->width()),
                   clipY(y - glyph.yMax / window->height()), 0.0f, 0.0f},
                  0);
  glyph.texture.use(GL_TEXTURE0);
  resources->text2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  x += glyph.advance / window->width();
}
}  // namespace

Background2D::Background2D(Texture2D &texture) noexcept : texture_(texture) {}

void Background2D::draw() noexcept {
  texture_.use(GL_TEXTURE0);
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
    : texture_(texture),
      vbo_(
          {
              clipX(x), clipY(y + scaleY(texture_)),  // bottom left pos
              0.0f, 0.0f,                             // bottom left tex

              clipX(x + scaleX(texture_)),
              clipY(y + scaleY(texture_)),  // bottom right pos
              1.0f, 0.0f,                   // bottom right tex

              clipX(x + scaleX(texture_)), clipY(y),  // top right pos
              1.0f, 1.0f,                             // top right tex

              clipX(x), clipY(y),  // top left pos
              0.0f, 1.0f,          // top left tex
          },
          GL_STATIC_DRAW),
      vao_(vbo_, resources->quadEBO, resources->quadAttributes) {}

void Image2D::draw() noexcept {
  texture_.use(GL_TEXTURE0);
  ScopeGuard guard = vao_.use();
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
      onTexture_(onTexture),
      offTexture_(offTexture),
      vbo_(
          {
              clipX(x), clipY(y + scaleY(onTexture_)),  // bottom left pos
              0.0f, 0.0f,                               // bottom left tex

              clipX(x + scaleX(onTexture_)),
              clipY(y + scaleY(onTexture_)),  // bottom right pos
              1.0f, 0.0f,                     // bottom right tex

              clipX(x + scaleX(onTexture_)), clipY(y),  // top right pos
              1.0f, 1.0f,                               // top right tex

              clipX(x), clipY(y),  // top left pos
              0.0f, 1.0f,          // top left tex
          },
          GL_STATIC_DRAW),
      vao_(vbo_, resources->quadEBO, resources->quadAttributes),
      left_(x * window->width()),
      right_((x + scaleX(onTexture_)) * window->width()),
      top_(y * window->height()),
      bottom_((y + scaleY(onTexture_)) * window->height()) {
  assert((onTexture_.width() == offTexture_.width()) &&
         "on and off textures must have the same width");
  assert((onTexture_.height() == offTexture_.height()) &&
         "on and off textures must have the same height");
}

void Button2D::draw() noexcept {
  (on ? onTexture_ : offTexture_).use(GL_TEXTURE0);
  ScopeGuard guard = vao_.use();
  resources->image2D.use();
  resources->image2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

bool Button2D::clicked(int32_t x, int32_t y) const noexcept {
  float scaledRadius = tex2Window(RADIUS);
  float innerLeft = left_ + scaledRadius;
  float innerRight = right_ - scaledRadius;
  float innerTop = top_ + scaledRadius;
  float innerBottom = bottom_ - scaledRadius;
  if (innerLeft <= x && x <= innerRight && top_ <= y && y <= bottom_)
    return true;
  else if (left_ <= x && x <= right_ && innerTop <= y && y <= innerBottom)
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
      font_(font),
      texture_(texture),
      colour_(colour),
      vbo_(
          {
              clipX(x), clipY(y + scaleY(texture_)),  // bottom left pos
              0.0f, 0.0f,                             // bottom left tex

              clipX(x + scaleX(texture_)),
              clipY(y + scaleY(texture_)),  // bottom right pos
              1.0f, 0.0f,                   // bottom right tex

              clipX(x + scaleX(texture_)), clipY(y),  // top right pos
              1.0f, 1.0f,                             // top right tex

              clipX(x), clipY(y),  // top left pos
              0.0f, 1.0f,          // top left tex
          },
          GL_STATIC_DRAW),
      vao_(vbo_, resources->quadEBO, resources->quadAttributes),
      left_(x * window->width()),
      right_((x + scaleX(texture_)) * window->width()),
      top_(y * window->height()),
      bottom_((y + scaleY(texture_)) * window->height()),
      preCursor_(),
      composition_(),
      postCursor_(),
      glyphVBO_(vector<float>(16), GL_DYNAMIC_DRAW),
      glyphVAO_(glyphVBO_, resources->quadEBO, resources->quadAttributes),
      cursorVBO_(vector<float>(4), GL_DYNAMIC_DRAW),
      cursorVAO_(cursorVBO_, resources->cursorEBO,
                 resources->cursorAttributes) {}

Textbox2D::operator std::u32string() const noexcept {
  return preCursor_ + composition_ + postCursor_;
}

void Textbox2D::draw() noexcept {
  texture_.use(GL_TEXTURE0);
  ScopeGuard guard = vao_.use();
  resources->image2D.use();
  resources->image2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  font_.setSize(bottom_ - top_ - 2.0f * tex2Window(RADIUS));

  Glyph const &bar = font_.glyph(U'|');
  float baseline = (bottom_ - tex2Window(RADIUS) + bar.yMin) / window->height();
  float left = (left_ + tex2Window(RADIUS)) / window->width();

  glyphVAO_.use(guard);
  resources->text2D.use();
  resources->text2D.setUniform("colour", colour_);
  for (char32_t const &c : preCursor_)
    drawChar(font_, glyphVBO_, left, baseline, c);
  for (char32_t const &c : composition_)
    drawChar(font_, glyphVBO_, left, baseline, c);
  float cursorPos = left;
  for (char32_t const &c : postCursor_)
    drawChar(font_, glyphVBO_, left, baseline, c);
  if (active) {
    cursorVAO_.use(guard);
    cursorVBO_.update({clipX(cursorPos),
                       clipY((bottom_ - tex2Window(RADIUS)) / window->height()),
                       clipX(cursorPos),
                       clipY((top_ + tex2Window(RADIUS)) / window->height())},
                      0);
    resources->solid2D.use();
    resources->solid2D.setUniform("colour", {0.0f, 0.0f, 0.0f, 1.0f});
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, nullptr);
  }

  // TODO: cursor blink
}

bool Textbox2D::clicked(int32_t x, int32_t y) const noexcept {
  float scaledRadius = tex2Window(RADIUS);
  float innerLeft = left_ + scaledRadius;
  float innerRight = right_ - scaledRadius;
  float innerTop = top_ + scaledRadius;
  float innerBottom = bottom_ - scaledRadius;
  if (innerLeft <= x && x <= innerRight && top_ <= y && y <= bottom_)
    return true;
  else if (left_ <= x && x <= right_ && innerTop <= y && y <= innerBottom)
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
  composition_ = text;
}

void Textbox2D::textInput(std::u32string const &text) noexcept {
  preCursor_ += text;
}

void Textbox2D::left() noexcept {
  postCursor_ = composition_ + postCursor_;
  composition_.clear();
  if (!preCursor_.empty()) {
    char32_t moved = preCursor_.back();
    preCursor_.pop_back();
    postCursor_ = moved + postCursor_;
  }
}

void Textbox2D::right() noexcept {
  preCursor_ += composition_;
  composition_.clear();
  if (!postCursor_.empty()) {
    char32_t moved = postCursor_.front();
    postCursor_.erase(0, 1);
    preCursor_ += moved;
  }
}

void Textbox2D::home() noexcept {
  postCursor_ = preCursor_ + composition_ + postCursor_;
  composition_.clear();
  preCursor_.clear();
}

void Textbox2D::end() noexcept {
  preCursor_ += composition_ + postCursor_;
  composition_.clear();
  postCursor_.clear();
}

void Textbox2D::backspace() noexcept {
  if (!composition_.empty()) {
    composition_.pop_back();
  } else if (!preCursor_.empty()) {
    preCursor_.pop_back();
  }
}

TextField2D TextField2D::centered(Font &font, Texture2D &texture,
                                  vec4 const &colour, float x,
                                  float y) noexcept {
  return TextField2D(font, texture, colour, x - scaleX(texture) / 2.0f,
                     y - scaleY(texture) / 2.0f);
}

TextField2D::TextField2D(Font &font, Texture2D &texture, vec4 const &colour,
                         float x, float y) noexcept
    : text(),
      font_(font),
      texture_(texture),
      colour_(colour),
      vbo_(
          {
              clipX(x), clipY(y + scaleY(texture_)),  // bottom left pos
              0.0f, 0.0f,                             // bottom left tex

              clipX(x + scaleX(texture_)),
              clipY(y + scaleY(texture_)),  // bottom right pos
              1.0f, 0.0f,                   // bottom right tex

              clipX(x + scaleX(texture_)), clipY(y),  // top right pos
              1.0f, 1.0f,                             // top right tex

              clipX(x), clipY(y),  // top left pos
              0.0f, 1.0f,          // top left tex
          },
          GL_STATIC_DRAW),
      vao_(vbo_, resources->quadEBO, resources->quadAttributes),
      left_(x * window->width()),
      right_((x + scaleX(texture_)) * window->width()),
      top_(y * window->height()),
      bottom_((y + scaleY(texture_)) * window->height()),
      glyphVBO_(vector<float>(16), GL_DYNAMIC_DRAW),
      glyphVAO_(glyphVBO_, resources->quadEBO, resources->quadAttributes) {}

void TextField2D::draw() noexcept {
  texture_.use(GL_TEXTURE0);
  ScopeGuard guard = vao_.use();
  resources->image2D.use();
  resources->image2D.setUniform("tex", 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  font_.setSize(bottom_ - top_ - 2.0f * tex2Window(RADIUS));

  Glyph const &bar = font_.glyph(U'|');
  float baseline = (bottom_ - tex2Window(RADIUS) + bar.yMin) / window->height();
  float left = (left_ + tex2Window(RADIUS)) / window->width();

  glyphVAO_.use(guard);
  resources->text2D.use();
  resources->text2D.setUniform("colour", colour_);
  for (char32_t const &c : text) drawChar(font_, glyphVBO_, left, baseline, c);
}

float layout(size_t index, size_t count) noexcept {
  return (index + 0.5f) / count;
}
}  // namespace airewar::ui
