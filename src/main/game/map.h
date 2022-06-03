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

#ifndef AIREWAR_GAME_MAP_H_
#define AIREWAR_GAME_MAP_H_

#include <array>
#include <cstdint>
#include <memory>

#include "glm/glm.hpp"

namespace airewar::game {
class Map final {
 public:
  struct Tile final {
    Tile() noexcept = default;
    Tile(Tile const &) noexcept = default;
    Tile(Tile &&) noexcept = default;

    ~Tile() noexcept = default;

    Tile &operator=(Tile const &) noexcept = default;
    Tile &operator=(Tile &&) noexcept = default;
  };

  /** radius of the world in meters (= 6,371 km) */
  static constexpr float RADIUS = 6'371'000.0f;

  /** maximum side length of a leaf node (= 50 km, ends up with 2621360
   * triangles) */
  static constexpr float MAX_TILE_SIZE = 50'000.0f;

  Map() noexcept = default;
  Map(Map const &) noexcept = default;
  Map(Map &&) noexcept = default;

  ~Map() noexcept = default;

  Map &operator=(Map const &) noexcept = default;
  Map &operator=(Map &&) noexcept = default;

  void initTriangles() noexcept;

  void generate(uint64_t) noexcept;
  uint64_t getSeed() const noexcept;

 private:
  class Node {
   public:
    Node() noexcept = default;
    Node(Node const &) noexcept = default;
    Node(Node &&) noexcept = default;

    virtual ~Node() noexcept = default;

    Node &operator=(Node const &) noexcept = default;
    Node &operator=(Node &&) noexcept = default;

    virtual void projectOntoSphere() noexcept = 0;
  };

  class IcosaNode final : public Node {
   public:
    IcosaNode() noexcept;
    IcosaNode(IcosaNode const &) noexcept = default;
    IcosaNode(IcosaNode &&) noexcept = default;

    ~IcosaNode() noexcept override = default;

    IcosaNode &operator=(IcosaNode const &) noexcept = default;
    IcosaNode &operator=(IcosaNode &&) noexcept = default;

    void projectOntoSphere() noexcept override;

   private:
    std::array<std::unique_ptr<Node>, 20> children_;
  };
  class TriangularNode : public Node {
   public:
    explicit TriangularNode(std::array<glm::vec3, 3> const &vertices) noexcept;
    TriangularNode(TriangularNode const &) noexcept = default;
    TriangularNode(TriangularNode &&) noexcept = default;

    ~TriangularNode() noexcept override = default;

    TriangularNode &operator=(TriangularNode const &) noexcept = default;
    TriangularNode &operator=(TriangularNode &&) noexcept = default;

    void projectOntoSphere() noexcept override;

   protected:
    std::array<glm::vec3, 3> vertices_;
  };
  class TriangleNode final : public TriangularNode {
   public:
    explicit TriangleNode(std::array<glm::vec3, 3> const &vertices) noexcept;
    TriangleNode(TriangleNode const &) noexcept = default;
    TriangleNode(TriangleNode &&) noexcept = default;

    ~TriangleNode() noexcept override = default;

    TriangleNode &operator=(TriangleNode const &) noexcept = default;
    TriangleNode &operator=(TriangleNode &&) noexcept = default;

    void projectOntoSphere() noexcept override;

   private:
    std::array<std::unique_ptr<Node>, 4> children_;
  };
  class LeafNode final : public TriangularNode {
   public:
    explicit LeafNode(std::array<glm::vec3, 3> const &vertices) noexcept;
    LeafNode(LeafNode const &) noexcept = default;
    LeafNode(LeafNode &&) noexcept = default;

    ~LeafNode() noexcept override = default;

    LeafNode &operator=(LeafNode const &) noexcept = default;
    LeafNode &operator=(LeafNode &&) noexcept = default;

   private:
    Tile tile_;
  };

  std::unique_ptr<Node> root_;
  uint64_t seed_;
};
}  // namespace airewar::game

#endif  // AIREWAR_GAME_MAP_H_
