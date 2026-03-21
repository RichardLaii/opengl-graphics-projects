# opengl-graphics-projects
# OpenGL Graphics and Shader Projects

A comprehensive collection of real-time rendering and GPU-based graphics projects implemented using OpenGL and GLSL.

This repository covers the full graphics pipeline, from transformations and lighting to advanced shader programming and physically-inspired simulations.

---

## 🧱 Graphics Fundamentals

### 🎠 Transformations & Animation
- Implemented hierarchical transformations (translation, rotation, scaling)
- Simulated carousel animation with multiple objects and circular motion
- Supported perspective and orthographic camera switching

### 💡 Lighting System
- Implemented OpenGL lighting pipeline (point light, spotlight)
- Dynamic light movement using sinusoidal functions
- Material control (color, shininess)

### ✈️ Keyframe Animation
- Built a time-based animation system
- Interpolated translation, rotation, and color over time
- Implemented third-person camera tracking

### 🌍 Texture Mapping
- Applied textures to 3D objects (planets)
- Used display lists for efficient rendering

---

## 🎨 Shader Programming

### 🟡 Procedural Shaders
- Generated ellipse patterns using mathematical equations
- Animated shader parameters using time and keyframes

### 🌊 Displacement & Bump Mapping
- Implemented ripple deformation:  
  Z = A cos(2πBr + C) e^{-Dr}
- Computed normals for lighting
- Added noise-based perturbations

### 🐉 Vertex Deformation & Texture Blending
- Deformed mesh using noise + time
- Blended multiple textures dynamically

### 🔵 Geometry Shader
- Converted polygonal mesh into a collection of spheres
- Explored GPU-based geometry amplification

### 🧰 Image Manipulation Toolbox
- Built shader-based image distortion and transformation system

---

## 🚀 Advanced Projects

### ☀️ Solar System Simulation
- Modeled planetary motion using Kepler’s Third Law
- Implemented orbit + self-rotation
- Used GLM for stable transformations
- Added lighting and texture mapping

### 🌊 Wave Simulation (Shader Final Project)
- Implemented multiple wave models:
  - Sinusoidal
  - Square wave
  - Sawtooth wave
  - Stokes wave (shallow water)
- Added temperature parameter to simulate water → ice transition
- Used cube mapping for environment reflection

---

## 🛠️ Technologies

- C++
- OpenGL (GLUT)
- GLSL (Vertex, Fragment, Geometry Shaders)
- GLM
- Real-time rendering
