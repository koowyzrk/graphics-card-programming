[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/O5bR1qTQ)
[![](../../actions/workflows/cpp_cmake.yml/badge.svg)](../../actions)

# OpenGLGP

An OpenGL/CMake course project with several small scenes and shared rendering utilities.

## Included scenes

- `Triangle`
- `Sierpinski`
- `Solar System`
- `Housing Estate`
- `Interactive Scene`

When the app starts, it prints a console menu and asks you to choose a scene.

## Requirements

- CMake 3.21 or newer
- A C++ compiler supported by the project
- Git
- A CMake-capable IDE or terminal workflow

On Windows, enable `Developer Mode` so CMake can create the resources symlink.

## Build

```bash
cmake -B build
cmake --build build
```

## Run

```bash
./build/src/OpenGLGP
```

On Windows, run `build/src/OpenGLGP.exe`.

## Project layout

- `src/core` - shared engine-style code
- `src/apps` - scene implementations
- `thirdparty` - external dependencies
- `src/apps/*/res` - shaders, textures, and models

![Preview](example.png)
