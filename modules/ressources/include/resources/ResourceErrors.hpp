/*
 *  ResourceErrors
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

#ifndef RESOURCE_ERRORS_HPP_
#define RESOURCE_ERRORS_HPP_

#include <cstdint>
#include <exception>
#include <string>

namespace resources
{
namespace error
{

class UnregisteredResource : public std::exception
{
  public:
    UnregisteredResource(const std::string &typename_str)
        : message("Resource type '" + typename_str + "' isn't registered!")
    {
    }
    ~UnregisteredResource()
    {
    }

    const char *what() const noexcept override
    {
        return message.c_str();
    }

  private:
    std::string message;
};

class ResourceNotFound : public std::exception
{
  public:
    ResourceNotFound(uint32_t id) : message("Resource ID " + std::to_string(id) + " not found!")
    {
    }

    ResourceNotFound(const std::string &path) : message("Resource path '" + path + "' not found!")
    {
    }

    ~ResourceNotFound()
    {
    }

    const char *what() const noexcept override
    {
        return message.c_str();
    }

  private:
    std::string message;
};
} // namespace error
} // namespace resources

#endif /* !RESOURCE_ERRORS_HPP_ */
