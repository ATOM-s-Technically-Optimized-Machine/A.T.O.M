/*
 *  ResourceLoaders
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

#ifndef RESOURCE_LOADERS_HPP_
#define RESOURCE_LOADERS_HPP_

#include "ResourceManager.hpp"
#include "ResourceTypes.hpp"

#include <string>

namespace Resources
{

/**
 * @brief Default loader for ImageData
 *
 * Uses stb_image to decode PNG, JPG, BMP, TGA, PSD, GIF, HDR, PIC, PNM.
 *
 * @param path The file path to the image
 * @return ImageData The decoded image data
 * @throw std::runtime_error if the file cannot be loaded
 */
ImageData loadImage(const std::string &path);

/**
 * @brief Default loader for MeshData
 *
 * Uses tinyobjloader to parse Wavefront OBJ files.
 * Produces an indexed mesh with positions, optional normals, and optional UVs.
 *
 * @param path The file path to the .obj file
 * @return MeshData The parsed mesh data
 * @throw std::runtime_error if the file cannot be parsed
 */
MeshData loadMesh(const std::string &path);

/**
 * @brief Default loader for ShaderData
 *
 * Reads a single shader file and determines whether it is a vertex or
 * fragment shader based on the file extension:
 *   .vert / .vs  => vertex shader
 *   .frag / .fs  => fragment shader
 *
 * For combined shader loading, pass a path without extension and the loader
 * will try path.vert + path.frag automatically.
 *
 * @param path The file path to the shader file or base name
 * @return ShaderData The loaded shader source code
 * @throw std::runtime_error if the file cannot be read
 */
ShaderData loadShader(const std::string &path);

/**
 * @brief Default loader for AudioData
 *
 * Uses dr_wav to decode WAV files into 32-bit float PCM samples.
 *
 * @param path The file path to the .wav file
 * @return AudioData The decoded audio data
 * @throw std::runtime_error if the file cannot be loaded
 */
AudioData loadAudio(const std::string &path);

/**
 * @brief Default loader for FontData
 *
 * Reads the raw binary contents of a .ttf or .otf font file.
 *
 * @param path The file path to the font file
 * @return FontData The raw font file bytes
 * @throw std::runtime_error if the file cannot be read
 */
FontData loadFont(const std::string &path);

/**
 * @brief Registers all built-in resource types and their default loaders
 *
 * After calling this, the ResourceManager can automatically load:
 *   - ImageData (PNG, JPG, BMP, TGA, PSD, GIF, HDR, PIC, PNM via stb_image)
 *   - MeshData  (OBJ via tinyobjloader)
 *   - ShaderData (GLSL .vert/.frag text files)
 *   - AudioData (WAV via dr_wav)
 *   - FontData  (TTF/OTF raw bytes)
 *
 * @param manager The ResourceManager to set up
 */
void registerDefaultLoaders(ResourceManager &manager);
} // namespace Resources

#endif /* !RESOURCE_LOADERS_HPP_ */
