# Atom Resources

A lightweight, typed resource management library for C++20. It provides centralised loading, caching, and lifetime management for common asset types (images, meshes, shaders, audio, fonts) with zero coupling to any graphics or audio API.

## Table of Contents

- [Design Overview](#design-overview)
- [Architecture](#architecture)
- [Built-in Resource Types](#built-in-resource-types)
- [Integration](#integration)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
  - [ResourceManager](#resourcemanager)
  - [ResourcePool](#resourcepool)
  - [Default Loaders](#default-loaders)
  - [Error Handling](#error-handling)
- [Custom Resource Types](#custom-resource-types)
- [Directory Scanning](#directory-scanning)
- [License](#license)

---

## Design Overview

| Principle | Detail |
|---|---|
| **GPU/API agnostic** | Every resource type stores raw CPU-side data (pixels, vertices, PCM samples…). The renderer or audio engine uploads to GPU/device buffers itself. |
| **Typed pools** | Each resource type `T` lives in its own `ResourcePool<T>`, giving O(1) ID-based access with no virtual dispatch on retrieval. |
| **Automatic dedup** | Storing/loading the same path twice returns the existing `ResourceID` — no duplicate allocations. |
| **ID recycling** | Released IDs are recycled via a stack, keeping the internal vector compact. |
| **Move-only storage** | Resources are moved in, never copied — safe for large textures, audio buffers, etc. |
| **Loader abstraction** | Register a `std::function<T(const std::string&)>` per type; after that `load()` handles caching automatically. |

## Architecture

```
ResourceManager
 ├── ResourcePool<ImageData>      ── ImageData entries
 ├── ResourcePool<MeshData>       ── MeshData entries
 ├── ResourcePool<ShaderData>     ── ShaderData entries
 ├── ResourcePool<AudioData>      ── AudioData entries
 └── ResourcePool<FontData>       ── FontData entries
```

- **`ResourceManager`** — top-level façade. Owns a dynamic vector of type-erased `IResourcePool*` pointers, one per registered type. Provides base-path resolution, type registration, and forwarding to the correct pool.
- **`ResourcePool<T>`** — flat vector of `std::optional<T>` entries indexed by `(ID - 1)`. An `unordered_map<string, ResourceID>` gives O(1) path-based dedup. An optional loader function enables `load(path)` semantics.
- **`ResourceTypeId`** — atomic counter that assigns a unique `uint8_t` ID to each C++ type the first time it is queried (via `get<T>()`).

## Built-in Resource Types

| Type | Contents | Default loader | Backend library |
|---|---|---|---|
| `ImageData` | `width`, `height`, `channels`, `pixels` (raw bytes) | `loadImage()` | stb_image |
| `MeshData` | Interleaved `vertices`, `indices`, layout flags | `loadMesh()` | tinyobjloader |
| `ShaderData` | `vertexSource`, `fragmentSource` (GLSL strings) | `loadShader()` | std I/O |
| `AudioData` | `sampleRate`, `channels`, `samples` (float PCM) | `loadAudio()` | dr_wav |
| `FontData` | `fileBytes` (raw TTF/OTF binary) | `loadFont()` | std I/O |

## Integration

This module is designed to live inside the **ATOM** project tree. The actual asset files (textures, models, shaders, sounds, fonts) live in a top-level `resources/` directory, separate from the module source code:

```
ATOM/
├── resources/                  ← runtime assets (textures, models, sounds…)
│   ├── textures/
│   │   ├── wall.png
│   │   └── floor.jpg
│   ├── models/
│   │   └── cube.obj
│   ├── shaders/
│   │   ├── basic.vert
│   │   └── basic.frag
│   ├── sounds/
│   │   └── click.wav
│   └── fonts/
│       └── mono.ttf
└── modules/
    └── resources/              ← this module (source code / library)
        ├── CMakeLists.txt
        ├── include/
        ├── src/
        └── README.md
```

Then point the `ResourceManager` at the assets directory:

```cpp
Resources::ResourceManager manager("resources/");
// or with an absolute path:
Resources::ResourceManager manager(projectRoot + "/resources/");
```

All calls to `load()`, `store()`, `scan()`, etc. will resolve paths relative to that base, e.g. `manager.load<ImageData>("textures/wall.png")` reads `resources/textures/wall.png`.

### As a CMake static library

From the parent project's `CMakeLists.txt` (or a `modules/CMakeLists.txt`):

```cmake
add_subdirectory(modules/resources)
target_link_libraries(your_target PRIVATE atom-resources)
```

This gives you the public include directory (`include/`) automatically.

### Requirements

- **C++20** (for the `ResourceType` concept and `std::filesystem`)
- **C11** (for the vendored C loader libraries)
- CMake 3.16+

### Vendored dependencies (no extra setup needed)

| Library | Purpose |
|---|---|
| [stb_image](https://github.com/nothings/stb) | Image decoding (PNG, JPG, BMP, TGA, …) |
| [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) | Wavefront OBJ parsing |
| [dr_wav](https://github.com/mackron/dr_libs) | WAV decoding to float PCM |

## Quick Start

### Minimal example — loading a texture

```cpp
#include <resources/ResourceLoaders.hpp>
#include <iostream>

int main() {
    // 1. Create a manager pointed at your assets folder
    Resources::ResourceManager manager("resources/");

    // 2. Register ImageData + its loader
    Resources::registerDefaultLoaders(manager);

    // 3. Load the texture (returns a cached ID)
    Resources::ResourceID id = manager.load<Resources::ImageData>("textures/wall.png");

    // 4. Use it
    const auto &img = manager.get<Resources::ImageData>(id);
    std::cout << img.width << "x" << img.height
              << " (" << img.channels << " channels, "
              << img.pixels.size() << " bytes)" << std::endl;

    // 5. Release when done
    manager.release<Resources::ImageData>(id);
    return 0;
}
```

### Broader example

```cpp
#include <resources/ResourceLoaders.hpp>

int main() {
    Resources::ResourceManager manager("resources/");

    // Register all built-in types + their default loaders in one call
    Resources::registerDefaultLoaders(manager);

    // Load resources — returns a cached ID if the path was already loaded
    Resources::ResourceID texId  = manager.load<Resources::ImageData>("textures/wall.png");
    Resources::ResourceID meshId = manager.load<Resources::MeshData>("models/cube.obj");
    Resources::ResourceID sndId  = manager.load<Resources::AudioData>("sounds/click.wav");

    // Retrieve by ID (O(1))
    const auto &img = manager.get<Resources::ImageData>(texId);
    // img.width, img.height, img.channels, img.pixels ...

    // Retrieve by path (hash-map lookup)
    const auto &mesh = manager.get<Resources::MeshData>("models/cube.obj");
    // mesh.vertices, mesh.indices, mesh.hasNormals ...

    // Release when done
    manager.release<Resources::ImageData>(texId);

    // Or release everything at once
    manager.releaseAll();
    return 0;
}
```

## API Reference

### ResourceManager

```cpp
#include <resources/ResourceManager.hpp>
```

| Method | Description |
|---|---|
| `ResourceManager(const std::string &basePath)` | Construct with a base directory for relative paths. |
| `setBasePath(path)` | Change the base directory at runtime. |
| `resolvePath(path)` | Prepend base path (no-op for absolute paths). |
| `registerResource<T>()` | Create a pool for type `T`. Returns `false` if already registered. |
| `isRegistered<T>()` | Check if a pool exists for type `T`. |
| `store<T>(path, T&&)` | Move a resource into the pool. Returns its ID (deduplicates by path). |
| `get<T>(ResourceID)` | Get a reference to a resource by ID. |
| `getByPath<T>(path)` | Get a reference to a resource by its path key. |
| `getIdByPath<T>(path)` | Get the ID for a loaded path. |
| `isLoaded<T>(path)` | Check whether a path is already cached. |
| `load<T>(path)` | Load via the registered loader, or return cached ID. |
| `setLoader<T>(fn)` | Register a `std::function<T(const std::string&)>` loader. |
| `release<T>(ResourceID)` | Release a single resource by ID. |
| `releaseByPath<T>(path)` | Release a single resource by path. |
| `releaseAll()` | Release every resource across all pools. |
| `scan<T>(dir, extensions, recursive)` | Batch-load all matching files from a directory. |
| `pool<T>()` | Direct access to the underlying `ResourcePool<T>`. |

### ResourcePool

```cpp
#include <resources/ResourcePool.hpp>
```

Each pool is a flat vector of `std::optional<T>` entries. Methods mirror `ResourceManager` but without the template parameter or base-path resolution:

`store()`, `get()`, `getByPath()`, `getIdByPath()`, `hasId()`, `hasPath()`, `release()`, `releaseByPath()`, `releaseAll()`, `count()`, `setLoader()`, `load()`.

### Default Loaders

```cpp
#include <resources/ResourceLoaders.hpp>
```

| Function | Loaded type | Supported formats |
|---|---|---|
| `loadImage(path)` | `ImageData` | PNG, JPG, BMP, TGA, PSD, GIF, HDR, PIC, PNM |
| `loadMesh(path)` | `MeshData` | Wavefront OBJ |
| `loadShader(path)` | `ShaderData` | `.vert`/`.vs`, `.frag`/`.fs`, or base name (auto pairs `.vert` + `.frag`) |
| `loadAudio(path)` | `AudioData` | WAV |
| `loadFont(path)` | `FontData` | TTF, OTF (raw bytes) |
| `registerDefaultLoaders(manager)` | — | Registers all five types and their loaders in one call. |

### Error Handling

All errors are thrown as exceptions defined in `ResourceErrors.hpp`:

| Exception | Thrown when |
|---|---|
| `ERROR::UnregisteredResource` | Accessing a pool for a type that hasn't been registered. |
| `ERROR::ResourceNotFound` | Looking up an invalid ID or an unknown path. |

## Custom Resource Types

Any move-constructible type can be used as a resource. Register it, optionally attach a loader, and use the same `store`/`get`/`load` API:

```cpp
struct LevelData {
    std::string name;
    std::vector<float> spawnPoints;
};

// Register the type
manager.registerResource<LevelData>();

// Option A: store manually
LevelData lvl;
lvl.name = "intro";
lvl.spawnPoints = {0.f, 0.f, 5.f, 10.f};
auto id = manager.store<LevelData>("levels/intro", std::move(lvl));

// Option B: register a loader for automatic caching
manager.setLoader<LevelData>([](const std::string &path) -> LevelData {
    // ... parse your custom format ...
    return loadedLevel;
});
auto id2 = manager.load<LevelData>("levels/world1");
```

## Directory Scanning

`scan<T>()` walks a directory, filters by extension, and calls `load<T>()` on each match. A loader must already be registered for the type.

```cpp
// Load all PNGs and JPGs from the textures folder (recursively)
auto ids = manager.scan<Resources::ImageData>("textures/", {".png", ".jpg"}, true);
```

## License

LGPL-2.1-or-later — see the license headers in source files for the full text.
