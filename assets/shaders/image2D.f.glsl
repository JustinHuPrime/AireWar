#version 430 core

// Copyright 2021 Justin Hu
//
// SPDX-License-Identifier: AGPL-3.0-or-later
//
// This file is part of AireWar.
//
// AireWar is free software: you can redistribute it and/or modify tt under the
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

layout(location = 0) out vec4 fragColour;

layout(location = 0) in vec2 texCoord_;

uniform sampler2D tex;

void main() { fragColour = texture(tex, texCoord_); }