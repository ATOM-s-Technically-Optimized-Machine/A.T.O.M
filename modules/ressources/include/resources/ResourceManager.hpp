/*
 *  ResourceManager
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

#ifndef RESOURCE_MANAGER_HPP_
#define RESOURCE_MANAGER_HPP_

#include "ResourceIncludes.hpp"
#include "ResourcePool.hpp"
#include "ResourceTypes.hpp"

#include <filesystem>
#include <functional>
#include <string>
#include <typeinfo>
#include <vector>

namespace resources
{

/**
 * @brief Top-level API for the typed resource management system
 *
 * Manages a dynamic vector of type-erased ResourcePool instances.
 * Each resource type T gets its own pool, identified by a runtime-generated
 * uint8_t type ID.
 *
 * The ResourceManager does NOT do any file I/O itself — it is purely typed
 * storage + caching + ID management. Consumers load data however they want
 * and call store(), or register a loader via setLoader() for convenience.
 */
class ResourceManager
{
  public:
    ResourceManager() : m_base_path("")
    {
    }

    /**
     * @brief Construct a new ResourceManager with a base resources directory
     *
     * All relative paths passed to store(), load(), getByPath(), etc.
     * will be resolved relative to this base path.
     *
     * @param basePath The root directory for resources (e.g. "assets/" or "resources/")
     */
    ResourceManager(const std::string &basePath) : m_base_path(basePath)
    {
        // Ensure trailing separator
        if (!m_base_path.empty() && m_base_path.back() != '/')
            m_base_path += '/';
    }

    ~ResourceManager()
    {
        for (auto *p : m_pools)
            delete p;
    }

    /**
     * @brief Sets the base resources directory
     *
     * All relative paths will be resolved against this directory.
     *
     * @param basePath The root directory for resources
     */
    void setBasePath(const std::string &basePath)
    {
        m_base_path = basePath;
        if (!m_base_path.empty() && m_base_path.back() != '/')
            m_base_path += '/';
    }

    /**
     * @brief Returns the current base resources directory
     *
     * @return const std::string& The base path
     */
    const std::string &getBasePath() const
    {
        return m_base_path;
    }

    /**
     * @brief Resolves a path relative to the base directory
     *
     * If the path is already absolute, returns it as-is.
     * If the base path is empty, returns the path as-is.
     * Otherwise, prepends the base path.
     *
     * @param path The relative or absolute path
     * @return std::string The resolved full path
     */
    std::string resolvePath(const std::string &path) const
    {
        if (m_base_path.empty())
            return path;
        if (!path.empty() && path[0] == '/')
            return path;
        return m_base_path + path;
    }

    /**
     * @brief Registers a new resource type in the manager
     *
     * Creates a ResourcePool<T> for this type. Can then be used
     * to store and retrieve resources of type T.
     *
     * @tparam T The resource type to register
     * @return true if the resource type has been registered successfully
     * @return false if the resource type was already registered
     */
    template <ResourceType T> bool registerResource()
    {
        uint8_t id = ResourceTypeId::get<T>();
        if (id < m_pools.size() && m_pools[id] != nullptr)
            return false;
        if (id >= m_pools.size())
            m_pools.resize(id + 1, nullptr);
        m_pools[id] = new ResourcePool<T>();
        return true;
    }

    /**
     * @brief Checks if a resource type is registered
     *
     * @tparam T The resource type to check for
     * @return true if it is registered
     * @return false if it is not
     */
    template <ResourceType T> bool isRegistered()
    {
        uint8_t id = ResourceTypeId::get<T>();
        return id < m_pools.size() && m_pools[id] != nullptr;
    }

    /**
     * @brief Stores a resource with the given path
     *
     * If the path is already stored for this type, returns the existing ID.
     * The data is moved in — no copies.
     *
     * @tparam T The resource type
     * @param path The file path or key identifying this resource
     * @param data The resource data to store (will be moved)
     * @return ResourceID The assigned or existing ID
     * @throw error::UnregisteredResource => if the resource type isn't registered
     */
    template <ResourceType T> ResourceID store(const std::string &path, T &&data)
    {
        return pool<T>().store(resolvePath(path), std::move(data));
    }

    /**
     * @brief Get a resource by its ID
     *
     * @tparam T The resource type
     * @param id The resource ID
     * @return T& Reference to the resource data
     * @throw error::UnregisteredResource => if the resource type isn't registered
     * @throw error::ResourceNotFound => if the ID is invalid or released
     */
    template <ResourceType T> T &get(ResourceID id)
    {
        return pool<T>().get(id);
    }

    /**
     * @brief Get a resource by its ID (const version)
     *
     * @tparam T The resource type
     * @param id The resource ID
     * @return const T& Const reference to the resource data
     * @throw error::UnregisteredResource => if the resource type isn't registered
     * @throw error::ResourceNotFound => if the ID is invalid or released
     */
    template <ResourceType T> const T &get(ResourceID id) const
    {
        return pool<T>().get(id);
    }

    /**
     * @brief Get a resource by its file path
     *
     * @tparam T The resource type
     * @param path The file path or key for the resource
     * @return T& Reference to the resource data
     * @throw error::UnregisteredResource => if the resource type isn't registered
     * @throw error::ResourceNotFound => if the path is not loaded
     */
    template <ResourceType T> T &getByPath(const std::string &path)
    {
        return pool<T>().getByPath(resolvePath(path));
    }

    /**
     * @brief Get the ID associated with a file path
     *
     * @tparam T The resource type
     * @param path The file path or key for the resource
     * @return ResourceID The ID of the resource
     * @throw error::UnregisteredResource => if the resource type isn't registered
     * @throw error::ResourceNotFound => if the path is not loaded
     */
    template <ResourceType T> ResourceID getIdByPath(const std::string &path)
    {
        return pool<T>().getIdByPath(resolvePath(path));
    }

    /**
     * @brief Checks if a resource with the given path is loaded for type T
     *
     * @tparam T The resource type
     * @param path The file path or key for the resource
     * @return true if the resource is loaded
     * @return false if it isn't
     */
    template <ResourceType T> bool isLoaded(const std::string &path)
    {
        return pool<T>().hasPath(resolvePath(path));
    }

    /**
     * @brief Releases a resource by its ID
     *
     * @tparam T The resource type
     * @param id The resource ID to release
     * @throw error::UnregisteredResource => if the resource type isn't registered
     */
    template <ResourceType T> void release(ResourceID id)
    {
        pool<T>().release(id);
    }

    /**
     * @brief Releases a resource by its file path
     *
     * @tparam T The resource type
     * @param path The file path or key of the resource to release
     * @throw error::UnregisteredResource => if the resource type isn't registered
     */
    template <ResourceType T> void releaseByPath(const std::string &path)
    {
        pool<T>().releaseByPath(resolvePath(path));
    }

    /**
     * @brief Releases all resources across all registered pools
     */
    void releaseAll()
    {
        for (auto *p : m_pools)
            if (p)
                p->releaseAll();
    }

    /**
     * @brief Registers a loader function for a resource type
     *
     * The loader takes a file path and returns a constructed resource.
     * Used by load<T>() for automatic load-or-return-cached behavior.
     *
     * @tparam T The resource type
     * @param loader A function that takes a path and returns T
     * @throw error::UnregisteredResource => if the resource type isn't registered
     */
    template <ResourceType T> void setLoader(std::function<T(const std::string &)> loader)
    {
        pool<T>().setLoader(std::move(loader));
    }

    /**
     * @brief Loads a resource by path using the registered loader
     *
     * If the path is already loaded, returns the existing ID.
     * If not, calls the registered loader to construct the resource
     * and stores it.
     *
     * @tparam T The resource type
     * @param path The file path to load
     * @return ResourceID The ID of the loaded resource
     * @throw error::UnregisteredResource => if the resource type isn't registered
     * @throw error::ResourceNotFound => if no loader is registered and the path isn't loaded
     */
    template <ResourceType T> ResourceID load(const std::string &path)
    {
        return pool<T>().load(resolvePath(path));
    }

    /**
     * @brief Get the pool for a specific resource type
     *
     * @tparam T The resource type
     * @return ResourcePool<T>& Reference to the pool
     * @throw error::UnregisteredResource => if the resource type isn't registered
     */
    template <ResourceType T> ResourcePool<T> &pool()
    {
        uint8_t id = ResourceTypeId::get<T>();
        if (id >= m_pools.size() || !m_pools[id])
            throw error::UnregisteredResource(typeid(T).name());
        return *static_cast<ResourcePool<T> *>(m_pools[id]);
    }

    /**
     * @brief Get the pool for a specific resource type (const version)
     *
     * @tparam T The resource type
     * @return const ResourcePool<T>& Const reference to the pool
     * @throw error::UnregisteredResource => if the resource type isn't registered
     */
    template <ResourceType T> const ResourcePool<T> &pool() const
    {
        uint8_t id = ResourceTypeId::get<T>();
        if (id >= m_pools.size() || !m_pools[id])
            throw error::UnregisteredResource(typeid(T).name());
        return *static_cast<const ResourcePool<T> *>(m_pools[id]);
    }

    /**
     * @brief Scans a directory and auto-loads all matching files for a resource type
     *
     * Walks the given directory (relative to base path), matches files by
     * extension, and calls load<T>() on each match. A loader must be
     * registered for this type beforehand via setLoader<T>().
     *
     * @tparam T The resource type to load
     * @param directory The directory to scan (relative to base path)
     * @param extensions List of file extensions to match (e.g. {".png", ".jpg"})
     * @param recursive Whether to scan subdirectories
     * @return std::vector<ResourceID> The IDs of all loaded resources
     */
    template <ResourceType T>
    std::vector<ResourceID> scan(const std::string &directory,
                                 const std::vector<std::string> &extensions, bool recursive = false)
    {
        std::vector<ResourceID> loaded;
        std::string fullDir = resolvePath(directory);

        if (!std::filesystem::exists(fullDir))
            return loaded;

        auto matchesExtension = [&extensions](const std::filesystem::path &p)
        {
            std::string ext = p.extension().string();
            for (const auto &e : extensions)
            {
                if (ext == e)
                    return true;
            }
            return false;
        };

        if (recursive)
        {
            for (const auto &entry : std::filesystem::recursive_directory_iterator(fullDir))
            {
                if (entry.is_regular_file() && matchesExtension(entry.path()))
                {
                    ResourceID id = load<T>(entry.path().string());
                    loaded.push_back(id);
                }
            }
        }
        else
        {
            for (const auto &entry : std::filesystem::directory_iterator(fullDir))
            {
                if (entry.is_regular_file() && matchesExtension(entry.path()))
                {
                    ResourceID id = load<T>(entry.path().string());
                    loaded.push_back(id);
                }
            }
        }
        return loaded;
    }

  private:
    /*
        Dynamic vector of type-erased pools, grows on demand.
        Indexed by the runtime type ID (uint8_t).
        Typically holds 3-10 entries — no fixed UINT16_MAX array needed.
    */
    std::vector<IResourcePool *> m_pools;

    // Base directory for all resource paths. Empty = no prefix.
    std::string m_base_path;
};
} // namespace resources

#endif /* !RESOURCE_MANAGER_HPP_ */
