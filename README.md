# CST-310 Project 5 — Realistic Scene Upgrade (OpenGL + GLUT + Shaders)

## Overview
This project is a **direct continuation of Project 4**, but the scene is upgraded to look significantly more realistic. Instead of relying only on basic GLUT primitives, the scene is rendered using a **cube mesh (VAO/VBO/EBO)** and a **custom GLSL shader pipeline** to add lighting + surface shading.

The scene recreates the same building-style layout:
- Large concrete ground base
- Tan tiled walkway in front of the entrance (with repeated tiles + slight variation)
- Left/right brick wall sections (brick detail vs. flat color from Project 4)
- Central glass/window tower (more detailed paneling)
- Entrance door structure

---

## Key Improvements vs. Project 4
- **Modern shader pipeline (GLSL 330 core)** with per-fragment lighting (ambient + diffuse + specular)
- **Mesh-based cube rendering** (cube vertices + normals + indexed triangles)
- **More detail/complexity** (brick patterning, tiled ground, window/door detailing)
- **Movable virtual camera** for viewing the scene dynamically

---

## Controls
- `W` / `S` → move forward/back (Z)
- `A` / `D` → move left/right (X)
- `Q` / `E` → move down/up (Y)
- `+` / `-` → zoom in/out (if enabled in your keyboard handler)
- `ESC` → quit

---

## Requirements (Ubuntu)
Install OpenGL/GLUT + GLEW:

```bash
sudo apt update
sudo apt install build-essential freeglut3-dev libglew-dev mesa-utils
