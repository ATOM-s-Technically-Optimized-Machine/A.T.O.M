/*
 *  ResourceTypes
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

#ifndef RESOURCE_TYPES_HPP_
#define RESOURCE_TYPES_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace resources
{

/**
 * @brief Raw decoded image data (pixels)
 *
 * Stores width, height, channel count and raw pixel bytes.
 * No GPU state — the renderer uploads this to a GL texture itself.
 */
struct ImageData
{
    int width = 0;
    int height = 0;
    int channels = 0;                  // 1 = grayscale, 3 = RGB, 4 = RGBA
    std::vector<unsigned char> pixels; // Raw decoded pixel data
};

/**
 * @brief Raw mesh geometry data
 *
 * Stores interleaved vertex data and index buffer.
 * The vertex layout depends on the flags (hasUVs, hasNormals, hasColors).
 * No VAO/VBO — the renderer creates GPU buffers from this itself.
 *
 * Vertex layout (floats per vertex):
 *  - position:  3 (always present)
 *  - normals:   3 (if hasNormals)
 *  - UVs:       2 (if hasUVs)
 *  - colors:    3 (if hasColors)
 */
struct MeshData
{
    std::vector<float> vertices;       // Interleaved vertex attributes
    std::vector<unsigned int> indices; // Index buffer
    bool hasUVs = false;
    bool hasNormals = false;
    bool hasColors = false;
};

/**
 * @brief Raw shader source code
 *
 * Stores the source strings for vertex and fragment shaders.
 * No compiled GL program — the renderer compiles and links itself.
 */
struct ShaderData
{
    std::string vertexSource;   // Vertex shader source code
    std::string fragmentSource; // Fragment shader source code
};

/**
 * @brief Raw decoded audio sample data
 *
 * Stores sample rate, channel count, and decoded PCM samples as floats.
 * No audio API buffers — the audio module uploads this itself.
 */
struct AudioData
{
    int sampleRate = 0;
    int channels = 0;           // 1 = mono, 2 = stereo
    std::vector<float> samples; // Interleaved PCM samples, normalized [-1, 1]
};

/**
 * @brief Raw font data
 *
 * Stores the raw font file bytes for use with a font rasterizer
 * (e.g. stb_truetype, FreeType). No glyph atlas — the text renderer
 * builds that itself.
 */
struct FontData
{
    std::vector<unsigned char> fileBytes; // Raw .ttf / .otf file contents
};
} // namespace resources

#endif /* !RESOURCE_TYPES_HPP_ */
