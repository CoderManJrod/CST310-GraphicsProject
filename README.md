# CST-310 Project 4 — Rendering a Scene with Primitives (OpenGL + GLUT)

## Overview
This program renders a simplified architectural scene using **OpenGL (fixed-function pipeline)** and **GLUT**.  
The scene is built from **scaled cubes** (primitives) to represent:
- Ground and tan tile floor reference
- Left/right brick walls
- Window tower with 3 stacked color sections
- Entrance recess and glass doors

A virtual camera is implemented using `gluLookAt`, and a perspective projection is implemented using `gluPerspective`.

---

## Features
- OpenGL + GLUT window
- Depth testing (`glEnable(GL_DEPTH_TEST)`)
- Perspective projection (`gluPerspective`)
- Camera/view setup (`gluLookAt`)
- Scene built from primitives (`glutSolidCube` + `glScalef`)

---

## Requirements (Ubuntu)
Install GLUT + OpenGL development packages:

```bash
sudo apt update
sudo apt install build-essential freeglut3-dev mesa-utils
