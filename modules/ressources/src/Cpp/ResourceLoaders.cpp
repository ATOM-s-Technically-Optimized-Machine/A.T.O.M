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

#include "resources/ResourceLoaders.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <unordered_map>

#include "stb_image.h"
#include "tiny_obj_loader.h"
#include "dr_wav.h"

namespace resources {

    ImageData loadImage(const std::string &path) {
        int w, h, ch;
        unsigned char *pixels = stbi_load(path.c_str(), &w, &h, &ch, 0);
        if (!pixels)
            throw std::runtime_error("Failed to load image: " + path
                                     + " (" + stbi_failure_reason() + ")");

        ImageData img;
        img.width = w;
        img.height = h;
        img.channels = ch;
        img.pixels.assign(pixels, pixels + (w * h * ch));
        stbi_image_free(pixels);
        return img;
    }

    MeshData loadMesh(const std::string &path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        std::string baseDir = std::filesystem::path(path).parent_path().string();
        if (!baseDir.empty()) baseDir += '/';

        bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                   path.c_str(), baseDir.c_str());
        if (!ok)
            throw std::runtime_error("Failed to load mesh: " + path + " " + err);

        MeshData mesh;
        mesh.hasNormals = !attrib.normals.empty();
        mesh.hasUVs = !attrib.texcoords.empty();
        mesh.hasColors = false;

        // Determine floats-per-vertex:  pos(3) + normals(3) + uvs(2)
        int stride = 3;
        if (mesh.hasNormals) stride += 3;
        if (mesh.hasUVs) stride += 2;

        // Flatten all shapes into a single indexed mesh with dedup via a map
        struct VertexKey {
            int vi, ni, ti;
            bool operator==(const VertexKey &o) const {
                return vi == o.vi && ni == o.ni && ti == o.ti;
            }
        };
        struct VertexKeyHash {
            std::size_t operator()(const VertexKey &k) const {
                std::size_t h = std::hash<int>()(k.vi);
                h ^= std::hash<int>()(k.ni) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<int>()(k.ti) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };
        std::unordered_map<VertexKey, unsigned int, VertexKeyHash> vertexMap;

        for (const auto &shape : shapes) {
            for (const auto &index : shape.mesh.indices) {
                VertexKey key{index.vertex_index, index.normal_index, index.texcoord_index};
                auto it = vertexMap.find(key);
                if (it != vertexMap.end()) {
                    mesh.indices.push_back(it->second);
                } else {
                    unsigned int newIdx = static_cast<unsigned int>(mesh.vertices.size() / stride);

                    // Position (always present)
                    mesh.vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                    mesh.vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                    mesh.vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

                    // Normals
                    if (mesh.hasNormals && index.normal_index >= 0) {
                        mesh.vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                        mesh.vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                        mesh.vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
                    } else if (mesh.hasNormals) {
                        mesh.vertices.push_back(0.0f);
                        mesh.vertices.push_back(0.0f);
                        mesh.vertices.push_back(0.0f);
                    }

                    // UVs
                    if (mesh.hasUVs && index.texcoord_index >= 0) {
                        mesh.vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                        mesh.vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
                    } else if (mesh.hasUVs) {
                        mesh.vertices.push_back(0.0f);
                        mesh.vertices.push_back(0.0f);
                    }

                    vertexMap[key] = newIdx;
                    mesh.indices.push_back(newIdx);
                }
            }
        }
        return mesh;
    }

    ShaderData loadShader(const std::string &path) {
        ShaderData shader;

        auto readFile = [](const std::string &filePath) -> std::string {
            std::ifstream file(filePath, std::ios::in);
            if (!file.is_open())
                throw std::runtime_error("Failed to open shader file: " + filePath);
            std::stringstream ss;
            ss << file.rdbuf();
            return ss.str();
        };

        std::string ext = std::filesystem::path(path).extension().string();

        if (ext == ".vert" || ext == ".vs") {
            shader.vertexSource = readFile(path);
        } else if (ext == ".frag" || ext == ".fs") {
            shader.fragmentSource = readFile(path);
        } else {
            // Try loading path.vert + path.frag as a pair
            std::string vertPath = path + ".vert";
            std::string fragPath = path + ".frag";
            bool hasVert = std::filesystem::exists(vertPath);
            bool hasFrag = std::filesystem::exists(fragPath);

            if (!hasVert && !hasFrag)
                throw std::runtime_error("Shader file not found: " + path
                                         + " (tried .vert/.frag)");
            if (hasVert) shader.vertexSource = readFile(vertPath);
            if (hasFrag) shader.fragmentSource = readFile(fragPath);
        }
        return shader;
    }

    AudioData loadAudio(const std::string &path) {
        drwav wav;
        if (!drwav_init_file(&wav, path.c_str(), nullptr))
            throw std::runtime_error("Failed to load audio: " + path);

        AudioData audio;
        audio.sampleRate = static_cast<int>(wav.sampleRate);
        audio.channels = static_cast<int>(wav.channels);

        drwav_uint64 totalFrames = wav.totalPCMFrameCount;
        drwav_uint64 totalSamples = totalFrames * wav.channels;

        audio.samples.resize(static_cast<std::size_t>(totalSamples));
        drwav_read_pcm_frames_f32(&wav, totalFrames, audio.samples.data());
        drwav_uninit(&wav);
        return audio;
    }

    FontData loadFont(const std::string &path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
            throw std::runtime_error("Failed to load font: " + path);

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        FontData font;
        font.fileBytes.resize(static_cast<std::size_t>(size));
        file.read(reinterpret_cast<char *>(font.fileBytes.data()), size);
        return font;
    }

    void registerDefaultLoaders(ResourceManager &manager) {
        manager.registerResource<ImageData>();
        manager.registerResource<MeshData>();
        manager.registerResource<ShaderData>();
        manager.registerResource<AudioData>();
        manager.registerResource<FontData>();

        manager.setLoader<ImageData>(loadImage);
        manager.setLoader<MeshData>(loadMesh);
        manager.setLoader<ShaderData>(loadShader);
        manager.setLoader<AudioData>(loadAudio);
        manager.setLoader<FontData>(loadFont);
    }
}
