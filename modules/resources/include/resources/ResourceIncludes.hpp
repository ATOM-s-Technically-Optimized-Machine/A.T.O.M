/*
 *  ResourceIncludes
 *
 *  Atom Resources is a lightweight typed resource management library
 *  Copyright (C) 2025 MUSIC Music
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#ifndef RESOURCE_INCLUDES_HPP_
#define RESOURCE_INCLUDES_HPP_

#include <atomic>
#include <cstdint>
#include <type_traits>

namespace resources
{

// Alias for uint32_t, used to represent and locate resources within a pool
using ResourceID = uint32_t;

// A resource ID of 0 is always invalid — valid IDs start at 1
constexpr ResourceID INVALID_RESOURCE = 0;

enum class AssetKind { Unknown, Texture, Mesh, Shader, Audio, Font, Scene };

// Concept for accepting resource types — resources must be movable
// (unlike ECS components which are default-constructible, resources
//  are built externally with real data and moved into the manager)
template <typename T>
concept ResourceType = std::is_move_constructible_v<T>;

/**
 * @brief Class for generating and getting unique type IDs per resource type
 *
 * Uses an atomic counter to assign a unique uint8_t ID to each
 * resource type the first time get<T>() is called.
 * uint8_t is used instead of uint16_t — you will never have 256 resource types.
 */
class ResourceTypeId
{
  private:
    inline static std::atomic<uint8_t> s_counter{0};

  public:
    /**
     * @brief Returns the ID attached to this specific type, generates an ID if this is the first
     * call
     *
     * @tparam T The type to get an ID from
     * @return uint8_t The corresponding ID
     */
    template <typename T> static uint8_t get()
    {
        static const uint8_t id = s_counter.fetch_add(1, std::memory_order_relaxed);
        return id;
    }
};
} // namespace resources

#endif /* !RESOURCE_INCLUDES_HPP_ */
