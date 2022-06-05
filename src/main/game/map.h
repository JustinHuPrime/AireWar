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
#include <functional>
#include <iterator>
#include <memory>
#include <vector>

#include "glm/glm.hpp"

namespace airewar::game {
class Map final {
 public:
  struct Plate;
  struct Tile final {
    glm::vec3 centroid;
    Plate const *plate;

    Tile(glm::vec3 const &centroid) noexcept;
    Tile(Tile const &) noexcept = default;
    Tile(Tile &&) noexcept = default;

    ~Tile() noexcept = default;

    Tile &operator=(Tile const &) noexcept = default;
    Tile &operator=(Tile &&) noexcept = default;
  };

  struct Plate {
    bool major;
    bool continental;
    Tile &center;

    Plate(bool major, bool continental, Tile &center) noexcept;
    Plate(Plate const &) noexcept = default;
    Plate(Plate &&) noexcept = default;

    ~Plate() noexcept = default;

    Plate &operator=(Plate const &) noexcept = default;
    Plate &operator=(Plate &&) noexcept = default;
  };
  std::vector<Plate> plates;

  /** radius of the world in meters (= 6,371 km) */
  static constexpr float RADIUS = 6'371'000.0f;

  /** maximum side length of a leaf node (= 50 km, ends up with 2621440
   * triangles) */
  static constexpr float MAX_TILE_SIZE = 50'000.0f;

  /** number of major plates to generate */
  static constexpr size_t NUM_MAJOR_PLATES = 8;

  /** number of minor plates to generate */
  static constexpr size_t NUM_MINOR_PLATES = 10;

  /** how much larger should major plates be */
  static constexpr float MAJOR_PLATE_SIZE_MULTIPLIER = 2.0f;

  // /** intersection checking tolerance */
  // static constexpr float INTERSECT_DELTA = 0.001f;

  Map() noexcept = default;
  Map(Map const &) noexcept = default;
  Map(Map &&) noexcept = default;

  ~Map() noexcept = default;

  Map &operator=(Map const &) noexcept = default;
  Map &operator=(Map &&) noexcept = default;

  void generate(uint64_t) noexcept;
  uint64_t getSeed() const noexcept;

  Tile &operator[](glm::vec3 const &ray) noexcept;

  size_t countTiles() const noexcept;

 private:
  struct Node {
    Node() noexcept = default;
    Node(Node const &) noexcept = default;
    Node(Node &&) noexcept = default;

    virtual ~Node() noexcept = default;

    Node &operator=(Node const &) noexcept = default;
    Node &operator=(Node &&) noexcept = default;

    virtual Tile &operator[](glm::vec3 const &ray) noexcept = 0;
    virtual Tile const &operator[](glm::vec3 const &ray) const noexcept = 0;

    virtual void projectOntoSphere() noexcept = 0;

    virtual bool intersectsRay(glm::vec3 const &ray) const noexcept = 0;

    virtual void forEach(std::function<void(Tile &)> const &) noexcept = 0;
  };

  struct TriangularNode;
  struct IcosaNode final : public Node {
    IcosaNode() noexcept;
    IcosaNode(IcosaNode const &) noexcept = default;
    IcosaNode(IcosaNode &&) noexcept = default;

    ~IcosaNode() noexcept override = default;

    IcosaNode &operator=(IcosaNode const &) noexcept = default;
    IcosaNode &operator=(IcosaNode &&) noexcept = default;

    Tile &operator[](glm::vec3 const &ray) noexcept override;
    Tile const &operator[](glm::vec3 const &ray) const noexcept override;

    void projectOntoSphere() noexcept override;

    bool intersectsRay(glm::vec3 const &ray) const noexcept override;

    void forEach(std::function<void(Tile &)> const &) noexcept override;

    std::array<std::unique_ptr<TriangularNode>, 20> children_;
  };
  struct TriangularNode : public Node {
    explicit TriangularNode(std::array<glm::vec3, 3> const &vertices) noexcept;
    TriangularNode(TriangularNode const &) noexcept = default;
    TriangularNode(TriangularNode &&) noexcept = default;

    ~TriangularNode() noexcept override = default;

    TriangularNode &operator=(TriangularNode const &) noexcept = default;
    TriangularNode &operator=(TriangularNode &&) noexcept = default;

    void projectOntoSphere() noexcept override;

    bool intersectsRay(glm::vec3 const &ray) const noexcept override;

    std::array<glm::vec3, 3> vertices_;
    glm::vec3 centroid;
  };
  struct TriangleNode final : public TriangularNode {
   public:
    explicit TriangleNode(std::array<glm::vec3, 3> const &vertices) noexcept;
    TriangleNode(TriangleNode const &) noexcept = default;
    TriangleNode(TriangleNode &&) noexcept = default;

    ~TriangleNode() noexcept override = default;

    TriangleNode &operator=(TriangleNode const &) noexcept = default;
    TriangleNode &operator=(TriangleNode &&) noexcept = default;

    Tile &operator[](glm::vec3 const &ray) noexcept override;
    Tile const &operator[](glm::vec3 const &ray) const noexcept override;

    void projectOntoSphere() noexcept override;

    void forEach(std::function<void(Tile &)> const &) noexcept override;

    std::array<std::unique_ptr<TriangularNode>, 4> children_;
  };
  struct LeafNode final : public TriangularNode {
    explicit LeafNode(std::array<glm::vec3, 3> const &vertices) noexcept;
    LeafNode(LeafNode const &) noexcept = default;
    LeafNode(LeafNode &&) noexcept = default;

    ~LeafNode() noexcept override = default;

    LeafNode &operator=(LeafNode const &) noexcept = default;
    LeafNode &operator=(LeafNode &&) noexcept = default;

    Tile &operator[](glm::vec3 const &ray) noexcept override;
    Tile const &operator[](glm::vec3 const &ray) const noexcept override;

    void projectOntoSphere() noexcept override;

    void forEach(std::function<void(Tile &)> const &) noexcept override;

    Tile tile_;
  };

  std::unique_ptr<IcosaNode> root_;
  uint64_t seed_;
};
}  // namespace airewar::game

#endif  // AIREWAR_GAME_MAP_H_
