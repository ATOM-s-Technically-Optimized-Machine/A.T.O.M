# A.T.O.M
A.T.O.M (ATOM's Technically Optimized Machine) is a game engine designed with optimization in mind, encouraging gamedevs to care about their players' machine. This is the main repo.

## Quick start

```bash
git clone --recurse-submodules git@github.com:ATOM-s-Technically-Optimized-Machine/A.T.O.M.git
cd A.T.O.M

cmake --preset dev        # configure  (Debug + ASan/UBSan, demos on)
cmake --build --preset dev

./build/dev/bin/atom-editor   # the editor (main target)
./build/dev/bin/atom-demo     # the renderer demo
```

Already cloned without `--recurse-submodules`? Run `git submodule update --init` (the ECS is a submodule, see [Repository layout](#repository-layout)).

## Build targets

| Target        | What it is                                       | Output                          |
|---------------|--------------------------------------------------|---------------------------------|
| `atom-editor` | **Main target.** ImGui-based editor (`modules/tools`) | `build/<preset>/bin/atom-editor` |
| `atom-demo`   | Renderer demo application (`demo/`), needs `ATOM_BUILD_DEMOS=ON` | `build/<preset>/bin/atom-demo`   |
| `atom-<module>` | One static library per module (`atom-core`, `atom-renderer`, ...) | `build/<preset>/modules/<module>/` |

Build a single target with `cmake --build --preset dev --target atom-editor`.

Every executable lands in `build/<preset>/bin/`. The editor and the demo find shaders and assets through the repository root compiled into them (`ATOM_ASSET_ROOT`), so they can be launched from any directory. ImGui writes its window layout to `imgui.ini` in the current directory (ignored by git).

## Presets

| Preset    | Build dir        | Settings                                                              |
|-----------|------------------|-----------------------------------------------------------------------|
| `dev`     | `build/dev/`     | Debug, `-g3`, AddressSanitizer + UndefinedBehaviorSanitizer, demos |
| `release` | `build/release/` | Release; demos off                                                    |

The `dev` preset exports `compile_commands.json` to `build/dev/`; symlink it to the repo root for clangd: `ln -s build/dev/compile_commands.json .`

### Useful options

| Option                    | Default | Effect                                    |
|---------------------------|---------|-------------------------------------------|
| `ATOM_BUILD_DEMOS`        | `OFF`   | Build `atom-demo` (`ON` in `dev`)         |
| `ATOM_INPUTS_BUILD_TESTS` | `OFF`   | Build the inputs module unit tests        |

## Requirements

- CMake >= 3.24 and a C++20 compiler (Clang or GCC; MSVC is untested)
- OpenGL
- GLFW 3 and GLM: used from the system if installed (e.g. `brew install glfw glm`), otherwise downloaded and built by CMake
- Dear ImGui (docking branch) is always downloaded by CMake

## Repository layout

```
.
├── CMakeLists.txt        root build: dependencies, modules, demo
├── CMakePresets.json     dev / release presets
├── modules/              engine modules, one static library each
│   ├── core/             core types and default components
│   │   └── include/core/ECS/   Blob-ECS (git submodule, header-only)
│   ├── renderer/         OpenGL renderer and render system
│   ├── ressources/       resource loading and caching (target atom-resources)
│   ├── inputs/           input events, state and cameras
│   ├── tools/            editor library + the atom-editor executable
│   ├── audio/  network/  physics/  script/   (placeholders)
├── demo/                 atom-demo
├── assets/               runtime assets (mostly not versioned)
└── build/                build trees, one per preset (not versioned)
```

All modules are regular directories of this repository. The only submodule is the ECS, [Blob-ECS](https://github.com/guilec06/Blob-ECS), mounted at `modules/core/include/core/ECS` so it is included as `#include "core/ECS/ECS.hpp"`. See [CONTRIBUTING.md](CONTRIBUTING.md#the-blob-ecs-submodule) for how to update it.
