/*
 *  ResourcePool
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

#ifndef RESOURCE_POOL_HPP_
#define RESOURCE_POOL_HPP_

#include "ResourceErrors.hpp"
#include "ResourceIncludes.hpp"

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Resources
{

/**
 * @brief ResourcePool interface, used for type-erased storage in ResourceManager
 *
 * This should be cast to a ResourcePool<T> to be used.
 */
class IResourcePool
{
  public:
    virtual ~IResourcePool() = default;

    /**
     * @brief Releases all resources stored in this pool
     */
    virtual void releaseAll() = 0;
};

/**
 * @brief ResourcePool that stores resources of the defined type
 *
 * Uses a flat vector of entries indexed by (ID - 1), with std::optional<T>
 * for proper destruction of released resources.
 * A hash map provides O(1) path-based dedup.
 * Freed IDs are recycled via a vector used as a stack (not std::queue,
 * which wraps std::deque and fragments memory).
 *
 * @tparam T The type of resource the pool is storing
 */
template <ResourceType T> class ResourcePool : public IResourcePool
{
  public:
    ResourcePool()
    {
    }
    ~ResourcePool()
    {
    }

    /**
     * @brief Stores a resource with the given path
     *
     * If the path is already stored, returns the existing ID (dedup / caching).
     * The data is moved in — no copies of potentially large resources.
     *
     * @param path The file path or key identifying this resource
     * @param data The resource data to store (will be moved)
     * @return ResourceID The assigned ID for the resource
     */
    ResourceID store(const std::string &path, T &&data)
    {
        auto it = m_path_index.find(path);
        if (it != m_path_index.end())
            return it->second;

        ResourceID id;
        if (!m_free_ids.empty())
        {
            id = m_free_ids.back();
            m_free_ids.pop_back();
            m_entries[id - 1] = Entry{std::move(data), path};
        }
        else
        {
            id = static_cast<ResourceID>(m_entries.size()) + 1;
            m_entries.push_back(Entry{std::move(data), path});
        }
        m_path_index[path] = id;
        return id;
    }

    /**
     * @brief Get the resource by its ID
     *
     * @param id The resource ID
     * @return T& Reference to the resource data
     * @throw ERROR::ResourceNotFound => if the ID is invalid or the resource was released
     */
    T &get(ResourceID id)
    {
        validateId(id);
        return m_entries[id - 1].data.value();
    }

    /**
     * @brief Get the resource by its ID (const version)
     *
     * @param id The resource ID
     * @return const T& Const reference to the resource data
     * @throw ERROR::ResourceNotFound => if the ID is invalid or the resource was released
     */
    const T &get(ResourceID id) const
    {
        validateId(id);
        return m_entries[id - 1].data.value();
    }

    /**
     * @brief Get the resource by its file path
     *
     * @param path The file path or key for the resource
     * @return T& Reference to the resource data
     * @throw ERROR::ResourceNotFound => if the path is not loaded
     */
    T &getByPath(const std::string &path)
    {
        return get(getIdByPath(path));
    }

    /**
     * @brief Get the resource by its file path (const version)
     *
     * @param path The file path or key for the resource
     * @return const T& Const reference to the resource data
     * @throw ERROR::ResourceNotFound => if the path is not loaded
     */
    const T &getByPath(const std::string &path) const
    {
        return get(getIdByPath(path));
    }

    /**
     * @brief Get the ID associated with a file path
     *
     * @param path The file path or key for the resource
     * @return ResourceID The ID of the resource
     * @throw ERROR::ResourceNotFound => if the path is not loaded
     */
    ResourceID getIdByPath(const std::string &path) const
    {
        auto it = m_path_index.find(path);
        if (it == m_path_index.end())
            throw ERROR::ResourceNotFound(path);
        return it->second;
    }

    /**
     * @brief Checks if a resource with the given ID exists and is alive
     *
     * @param id The resource ID
     * @return true if the resource exists
     * @return false if it doesn't
     */
    bool hasId(ResourceID id) const
    {
        return id >= 1 && id <= static_cast<ResourceID>(m_entries.size()) &&
               m_entries[id - 1].data.has_value();
    }

    /**
     * @brief Checks if a resource with the given path is loaded
     *
     * @param path The file path or key for the resource
     * @return true if the resource is loaded
     * @return false if it isn't
     */
    bool hasPath(const std::string &path) const
    {
        return m_path_index.count(path) > 0;
    }

    /**
     * @brief Releases a resource by its ID
     *
     * Properly destructs the resource data via std::optional::reset(),
     * removes the path index entry, and recycles the ID for reuse.
     *
     * @param id The resource ID to release
     */
    void release(ResourceID id)
    {
        if (!hasId(id))
            return;
        auto &entry = m_entries[id - 1];
        m_path_index.erase(entry.path);
        entry.data.reset();
        entry.path.clear();
        m_free_ids.push_back(id);
    }

    /**
     * @brief Releases a resource by its file path
     *
     * @param path The file path or key of the resource to release
     */
    void releaseByPath(const std::string &path)
    {
        auto it = m_path_index.find(path);
        if (it == m_path_index.end())
            return;
        release(it->second);
    }

    /**
     * @brief Releases all resources stored in this pool
     *
     * Clears all entries, path indices, and recycled IDs.
     */
    void releaseAll() override
    {
        m_entries.clear();
        m_path_index.clear();
        m_free_ids.clear();
    }

    /**
     * @brief Returns the number of currently loaded resources in this pool
     *
     * @return std::size_t The count of active resources
     */
    std::size_t count() const
    {
        return m_path_index.size();
    }

    /**
     * @brief Registers a loader function for this resource type
     *
     * The loader takes a file path and returns a constructed resource.
     * Used by load() for automatic load-or-return-cached behavior.
     *
     * @param loader A function that takes a path and returns T
     */
    void setLoader(std::function<T(const std::string &)> loader)
    {
        m_loader = std::move(loader);
    }

    /**
     * @brief Loads a resource by path using the registered loader
     *
     * If the path is already loaded, returns the existing ID.
     * If not, calls the registered loader to construct the resource
     * and stores it.
     *
     * @param path The file path to load
     * @return ResourceID The ID of the loaded resource
     * @throw ERROR::ResourceNotFound => if no loader is registered and the path isn't loaded
     */
    ResourceID load(const std::string &path)
    {
        if (hasPath(path))
            return getIdByPath(path);
        if (!m_loader)
            throw ERROR::ResourceNotFound(path);
        return store(path, m_loader(path));
    }

  private:
    /*
        An entry stores the resource data inside an std::optional
        along with the file path that identifies it.

        std::optional<T> is used instead of a bool alive flag because:
        - T's destructor runs properly when reset() is called (important
          for multi-MB resources like textures or audio buffers)
        - has_value() is the alive check — single source of truth
        - No zombie objects sitting in memory with alive=false
    */
    struct Entry
    {
        std::optional<T> data;
        std::string path;
    };

    std::vector<Entry> m_entries;                             // Indexed by (id - 1)
    std::unordered_map<std::string, ResourceID> m_path_index; // Path -> ID dedup
    std::vector<ResourceID> m_free_ids;                       // Recycled IDs (vector as stack)
    std::function<T(const std::string &)> m_loader;           // Optional loader function

    /**
     * @brief Validates that a resource ID is valid and alive
     *
     * @param id The resource ID to validate
     * @throw ERROR::ResourceNotFound => if the ID is invalid or released
     */
    void validateId(ResourceID id) const
    {
        if (!hasId(id))
            throw ERROR::ResourceNotFound(id);
    }
};
} // namespace Resources

#endif /* !RESOURCE_POOL_HPP_ */
