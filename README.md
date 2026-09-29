# 🌆 Deferred & Multi-Pass Rendering

A real-time OpenGL deferred renderer and post-processing pipeline featuring off-screen G-Buffer generation, Blinn-Phong lighting, HDR cubemap environment compositing, velocity-driven rotational motion blur, automated log-average luminance mipmap reduction, and Reinhard photographic tone mapping[cite: 4].

Developed for **CENG 469: Computer Graphics II** at METU[cite: 4].

---

## 📖 Technical Report & Documentation

Read the complete technical breakdown, pipeline architecture, and implementation details:

👉 **[Read the Full Blog Post]([https://esatcivitci.github.io/Deffered-Multi-Pass-Rendering/](https://github.com/EsatCivitci/Deffered-Multi-Pass-Rendering/blob/main/docs/index.md))**

---

## 🎬 Demo Video

[![Watch the Demo Video](https://img.youtube.com/vi/Px7lX-ZELiA/maxresdefault.jpg)](https://www.youtube.com/watch?v=Px7lX-ZELiA)

👉 **[Click to watch the full demo on YouTube](https://www.youtube.com/watch?v=Px7lX-ZELiA)**

---

## 🛠️ Key Features

* **G-Buffer Geometry Pass**: Off-screen MRT storage for world-space positions and interpolated surface normals[cite: 4].
* **Deferred Lighting**: Blinn-Phong illumination evaluated per visible fragment with dynamic exposure control[cite: 4].
* **HDR Environment Compositing**: Camera-centered HDR cubemap integration blended seamlessly behind scene geometry[cite: 4].
* **Rotational Motion Blur**: Velocity-based box filter convolution driven by mouse rotation with gradual deceleration decay[cite: 4].
* **Luminance Reduction & Tone Mapping**: $1 \times 1$ mipmap hardware pyramid to extract scene geometric mean luminance for global Reinhard tone mapping and gamma correction ($\gamma = 2.2$)[cite: 4].
* **Inspection Modes**: Interactive debug toggles to view intermediate G-Buffer targets, blur buffers, and final tone-mapped outputs[cite: 4].

---

## 🚀 Build & Run

```bash
make
./main
