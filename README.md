# Ray Tracer

A physically-based, CPU-bound ray tracer written in modern C++ (C++17). It simulates light transport to produce highly realistic images featuring global illumination, soft shadows, caustics, depth of field, and true reflection/refraction. 

Instead of waiting for a final image to render to disk, the project uses a vendored GLFW + OpenGL setup to display a progressive, live-updating window. You can watch the ray tracer resolve the image row-by-row in real-time.

## Features & Implementation Details

| Feature | Implementation Notes |
| --- | --- |
| **BVH (Bounding Volume Hierarchy)** | Accelerates ray-object intersection from $O(n)$ to $O(\log n)$. Essential for complex scenes. |
| **Live Progressive Preview** | Uses a lightweight GLFW/OpenGL context to blit completed scanlines to a texture. You see results immediately. |
| **Multi-threading** | Work is split by scanlines across available CPU cores using `std::thread`. A lock-free atomic counter tracks progress. |
| **Anti-Aliasing** | Jittered sub-pixel sampling prevents jagged edges and averages out Monte Carlo noise. |
| **Russian Roulette** | Unbiased path termination. Randomly kills deep, low-contribution rays to save computation without darkening the image. |
| **Physically-Based Camera** | Supports adjustable field of view, defocus blur (depth of field) via a thin-lens approximation, and focal distance. |
| **HDR Tone Mapping** | Uses the ACES filmic tone mapping curve with 2.2 gamma correction to gracefully handle overexposed highlights without clipping. |
| **Thread-Local RNG** | Each worker thread maintains its own independent `std::mt19937` random number generator to prevent lock contention and correlation artifacts. |
| **Atmospheric Sky Model** | Provides a dynamic zenith-to-horizon gradient with a sun disc and warm ground glow for outdoor scenes. |

## Supported Materials

The material system uses polymorphism (`Material` base class) to handle different scattering behaviors seamlessly.

*   **Lambertian (Diffuse):** Matte surfaces. Scatters rays in random cosine-weighted directions.
*   **Metal (Conductor):** Reflective surfaces. Features a `fuzz` parameter to simulate microscopic roughness (from perfect mirrors to brushed aluminum).
*   **Dielectric (Insulator):** Glass, water, and diamonds. Implements Snell's Law for refraction, total internal reflection, and Schlick's approximation for angle-dependent reflectivity (Fresnel effect).
*   **DiffuseLight (Emissive):** Light sources. Emits radiance instead of scattering incoming rays. Essential for indoor scenes, soft shadows, and global illumination.

## Project Structure

```
.
├── CMakeLists.txt         # Build configuration
├── src/                   # C++ headers and source files
└── third_party/glfw/      # Vendored GLFW library (no system install needed)
```

### Core Architecture

1.  `Ray.h` / `Math.h`: The mathematical foundation (Vectors, Matrices, Parametric Rays).
2.  `Hittable.h` / `HittableList.h`: The geometry interface and scene graph.
3.  `Sphere.cpp`: Analytic sphere intersections. (Negative radii elegantly handle hollow glass by flipping normals).
4.  `BVH.cpp`: Axis-Aligned Bounding Boxes and the spatial acceleration tree.
5.  `Material.h`: The BSDF implementations governing light-surface interactions.
6.  `Camera.cpp`: Generates the primary rays for each pixel.
7.  `Renderer.cpp`: The main integrator. Handles the recursive ray-color logic, multi-threading, and tone mapping.
8.  `Window.cpp`: The GLFW display wrapper.
9.  `main.cpp`: Scene definitions and the execution entry point.

## Building the Project

The project requires CMake (3.10+), a C++17 compliant compiler, and standard system OpenGL libraries. GLFW is included directly in the repository.

```bash
# Configure the build in Release mode (critical for performance)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Compile the executable
cmake --build build -j
```

*Note for macOS users: You may see deprecation warnings regarding the OpenGL headers. This is standard for Apple's legacy OpenGL framework and does not affect the program.*

## Running the Ray Tracer

The executable takes a single integer argument (1-5) to select the scene. 

```bash
cd build
./RayTracer 1    # Cornell Box
./RayTracer 2    # Solar System
./RayTracer 3    # Hall of Mirrors
./RayTracer 4    # Underwater
./RayTracer 5    # Night Sky
```

By default, the renderer runs at **800×450**. The sample count and maximum bounce depth are tuned per scene. Let the window remain open until the terminal reports 100% completion. The final image is automatically saved to `output/render_scene_<id>.ppm`.

### Scene Descriptions

*   **Scene 1: Cornell Box**
    The classic ray tracing benchmark. Tests color bleeding (radiosity) from colored walls onto neutral objects, soft shadows from an area light, and enclosed global illumination.
*   **Scene 2: Solar System**
    Features a bright emissive sun at the origin surrounded by a rocky planet (Lambertian), a gas giant (Metal), and an icy moon (Dielectric).
*   **Scene 3: Hall of Mirrors**
    A corridor constructed from perfect mirrors. Tests the maximum recursion depth limit and showcases infinite recursive reflections.
*   **Scene 4: Underwater**
    A massive dielectric sphere representing water, acting as a giant lens that envelops and refracts objects placed inside it.
*   **Scene 5: Night Sky**
    A dark environment illuminated by hundreds of tiny emissive star spheres and a single large moon sphere, demonstrating handling of many distinct light sources.

## Performance Expectations

Ray tracing performance is bounded by `width × height × samples × average bounces`. 
Geometry intersection is heavily optimized by the BVH, making material complexity (e.g., highly recursive glass or mirrors) the primary bottleneck.

*   **Indoor / Emissive Scenes (Scenes 1, 5):** Require more samples to resolve noise from indirect lighting.
*   **Mirror / Glass Scenes (Scenes 3, 4):** Require higher bounce depths, keeping rays alive longer.

**Always build in `Release` mode.** Debug builds will run exponentially slower due to the lack of compiler optimizations for vector math and recursive functions.
