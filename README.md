# Graphics-engine
A software graphics engine written in C++. Renders 2D primitives and 3D meshes using a CPU rasterizer. Supports both SDL2/OpenGL and ncurses display backends.

## Features
- CPU software renderer

- Framebuffer-based rendering

- SDL2 + OpenGL presenter

- ncurses terminal renderer

- 2D drawing primitives
  - Pixels
  - Lines
  - Wireframe triangles
  - Filled triangles
  - Sprites

- 3D renderer
  - Matrix transforms
  - Translation
  - Rotation
  - Scaling
  - Perspective projection
  - Perspective divide
  - Viewport transform
  - Back-face culling
  - OBJ model loading
  - Optional textured rendering (.tga)
  - Wireframe rendering when no texture is supplied

## Requirements
- C++17 or newer
- SDL2
- OpenGL
- Linux or WSL

## Quick Start

Compile one of the example programs.

```
cd examples
chmod +x run.sh
./run.sh
```

Select a model from the menu.

Available examples:

```
Chess King (textured)
Dragon
Elephant
Monkey
Sponza
Teapot
Three Objects
Venus
```

## Project Structure

| Directory | Description |
|-----------|-------------|
| src/framebuffer | Framebuffer and rasterization routines |
| src/display | SDL2/OpenGL and ncurses display backends |
| src/render3D | 3D rendering pipeline |
| src/matrix | Matrix mathematics and transforms |
| src/objects | Meshes, OBJ loader and built-in primitives |
| src/textures | TGA texture loader |
| examples | Example programs and model viewer |
| models | Example OBJ models and textures |

## Rendering Pipeline

Each mesh is rendered using the following pipeline:

```
Model Space
    ↓
Scale
    ↓
Rotation
    ↓
Translation
    ↓
Back-face Culling
    ↓
Perspective Projection
    ↓
Perspective Divide
    ↓
Viewport Transform
    ↓
Rasterization
```

## Supported Formats

### Models

- OBJ

### Textures

- Uncompressed TGA
- Wireframe rendering when no texture is supplied

## Limitations

- CPU renderer only

- Painter's algorithm (no Z-buffer)

- No lighting

- No clipping against the near plane

- Only one texture per mesh

## Future Work

- Depth buffer

- Near-plane clipping

- Perspective-correct texture mapping improvements

- Lighting

- Normal maps

- Multiple materials

- Camera class

- Scene graph

- Frustum clipping

## Third-party Assets

Example models are from:

https://github.com/McNopper/OpenGL

Licensed under the MIT License.

The MIT License (MIT)

Copyright (c) since 2013 Norbert Nopper

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

## Why this exists

Built to understand how modern graphics pipelines work from first principles. The engine performs all transformations and rasterization on the CPU before presenting the framebuffer through SDL2/OpenGL or directly in the terminal using ncurses.# Graphics-engine
A graphics engine created in C++


Models from:
https://github.com/McNopper/OpenGL

(MIT):
The MIT License (MIT)

Copyright (c) since 2013 Norbert Nopper

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


