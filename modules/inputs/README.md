# ATOM Input Module

Backend-agnostic input handling module for the [A.T.O.M](https://github.com/ATOM-s-Technically-Optimized-Machine/A.T.O.M) game engine.

This module provides event-driven keyboard/mouse input, state tracking with edge detection, and camera controllers — all in pure C11 with zero platform dependencies.

## Features

- **Event system** — Circular event queue with callback listeners
- **Input state** — Double-buffered keyboard/mouse state with `just_pressed`, `just_released` edge detection
- **FPS camera** — Free-fly controller with WASD + mouse look, sprint, and vertical movement
- **Orbital camera** — Orbit, pan, and zoom around a focal point
- **Math library** — Vectors, quaternions, and 4x4 matrices (column-major, OpenGL-compatible)
- **Normalized movement** — Built-in WASD movement vectors that prevent diagonal speed boost

## Architecture

This module is a **pure event consumer**. It does not poll platform events or create windows — that responsibility belongs to the platform/window module.

```
[Platform Module]                [Input Module]
       │                              │
  Poll OS events                      │
  Convert to atom_input_event_t       │
       │                              │
       ├── push events ──────────────►│
       │                        process_events()
       │                        update_state()
       │                        update_cameras()
       │                              │
       │                              ├── queries (key_is_down, etc.)
       │                              └── camera matrices
```

## Integration

### As a git submodule

```bash
git submodule add <repo-url> modules/atom-inputs
```

### CMake

```cmake
add_subdirectory(modules/atom-inputs)
target_link_libraries(your-target PRIVATE atom-inputs)
```

## Usage

### Basic setup

```c
#include "inputs/inputs.h"

atom_input_system_t input;
atom_input_init(&input);

// Game loop
while (!atom_input_should_quit(&input)) {
    // 1. Push events from your platform module
    atom_event_queue_push(&input.event_queue, &your_event);

    // 2. Process events and update state
    atom_input_update(&input, platform_get_time_ms());

    // 3. Query input state
    const atom_input_state_t *state = atom_input_get_state(&input);

    if (atom_key_just_pressed(state, ATOM_KEY_ESCAPE))
        break;

    atom_vec3_t move = atom_input_get_movement_vector(state);

    // 4. End frame
    atom_input_end_frame(&input);
}

atom_input_shutdown(&input);
```

### FPS Camera

```c
atom_camera_t camera;
atom_fps_camera_bindings_t bindings;

atom_fps_camera_init(&camera, &bindings);

// Each frame, after atom_input_update():
atom_fps_camera_update(&camera, state, delta_time);

// Get view/projection matrices for rendering
const atom_mat4_t *view = atom_camera_get_view_matrix(&camera);
const atom_mat4_t *proj = atom_camera_get_projection_matrix(&camera);
```

### Orbital Camera

```c
atom_camera_t camera;
atom_orbital_camera_bindings_t bindings;

atom_vec3_t target = {0.0f, 0.0f, 0.0f};
atom_orbital_camera_init(&camera, &bindings, target, 5.0f);

// Each frame:
atom_orbital_camera_update(&camera, state, delta_time);
```

### Event Listeners

```c
void on_key(const atom_input_event_t *event, void *user_data) {
    printf("Key: %s\n", atom_keycode_name(event->data.key.key));
}

atom_input_add_listener(&input, ATOM_EVENT_KEY_DOWN, on_key, NULL);
```

## Building

```bash
mkdir build && cd build
cmake ..
make
```

Produces `libatom-inputs.a` — a static library with no external dependencies (only `libm` on Unix).

## Module structure

```
include/inputs/
├── inputs.h             Main API (system lifecycle, event processing)
├── input_event.h        Event types, queue, and listener system
├── input_state.h        Keyboard/mouse state tracking and queries
├── atom_math.h          Vector, quaternion, and matrix math
├── camera.h             Base camera abstraction
├── camera_fps.h         FPS camera controller
└── camera_orbital.h     Orbital camera controller

src/C/
├── inputs.c             System orchestration
├── input_event.c        Event queue and listener implementation
├── input_state.c        State update and query logic
├── atom_math.c          Math operations
├── camera.c             Camera state and matrix management
├── camera_fps.c         FPS controller logic
└── camera_orbital.c     Orbital controller logic
```

## License

Part of the [A.T.O.M](https://github.com/ATOM-s-Technically-Optimized-Machine/A.T.O.M) game engine — licensed under LGPL-2.1.
