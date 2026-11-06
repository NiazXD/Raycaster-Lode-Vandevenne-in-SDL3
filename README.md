# SDL3 Raycaster

A textured Wolfenstein 3D-style raycasting engine implemented in C++ using SDL3.

This project is a port of [Lode Vandevenne's Raycasting Tutorial](https://lodev.org/cgtutor/raycasting.html), originally written around SDL 1.2 and pseudocode. The implementation has been adapted to SDL3, translating the rendering logic into a working C++ application.

The project implements a classic 2.5D raycasting renderer, featuring textured walls, floors, and ceilings.

## Features

- **DDA Raycasting:** Grid-based ray traversal using the Digital Differential Analyzer algorithm.
- **Textured Walls:** Texture mapping with perspective-correct wall projection.
- **Floor Casting:** Per-pixel floor rendering using floor casting and perspective interpolation.
- **Ceiling Casting:** Textured ceiling rendering.
- **Software Rendering:** Direct pixel manipulation through a framebuffer.
- **SDL3 Integration:** Texture streaming and rendering using SDL3.
- **Player Movement:** First-person movement and camera rotation.

## Technologies

- C++
- SDL3
- Raycasting
- Software Rendering
- Computer Graphics

## Acknowledgements

This project is based on [Lode's Computer Graphics Tutorial – Raycasting](https://lodev.org/cgtutor/raycasting.html).

The original tutorial provided the mathematical foundations and rendering algorithms. This project adapts those concepts into a complete SDL3 implementation.

## Purpose

Built as a hands-on exploration of computer graphics, software rendering, and the mathematics behind early 3D game engines.

Rather than relying on a high-level graphics API, this project explores how a 2.5D scene can be constructed from individual pixels and mathematical calculations.
