# CPU Path Tracer

A from-scratch C++17 path tracer. It shoots camera rays into a scene of spheres (and optional triangles), bounces them according to physically based materials, and shows the image in a GLFW window while it converges.

This is an educational renderer, not a production engine. The code is small on purpose: one idea per file, so you can read the pipeline top to bottom.

## What it does

- Unbiased-style **path tracing** with cosine-weighted Lambertian scatter, mirror/fuzzy metal, and dielectric refraction (Snell + Schlick)
- **Emissive surfaces** (`DiffuseLight`) so rooms can be lit by geometry instead of a fake directional light
- A **BVH** over object AABBs so hundreds of random spheres stay interactive to trace
- A thin-lens **camera** (aperture + focus distance) for depth of field
- **Scanline multithreading** across `std::thread::hardware_concurrency()` cores
- A live **OpenGL texture** so you watch rows fill in instead of staring at a blank terminal
- **ACES filmic tone mapping** and gamma 2.2 so HDR samples look like a photograph, not a clipped PNG

## How a pixel is made

```
Camera ray  →  BVH hit  →  material scatter or emit  →  recurse
     ↑                                                    │
     └──────── average many samples, tone-map, display ───┘
```

1. For each pixel, the renderer fires `N` rays with a random offset inside the pixel (**jittered sampling**). That antialiases edges and averages noise.
2. `scene.hit` walks the BVH and returns the nearest intersection (`HitRecord`: point, normal, `t`, material).
3. The hit material either **emits** light, **scatters** a new ray (with an RGB attenuation), or both.
4. Recursion continues until `maxDepth` or **Russian roulette** kills a dim path.
5. Misses return an environment color (day sky, night sky, or black).
6. Samples are averaged, clamped, tone-mapped, and uploaded to the window.

`Renderer::rayColor` in `src/Renderer.cpp` is the whole integrator. Start there if you want to understand lighting.

## Why certain choices exist

These are the decisions that look arbitrary until you hit the bug they prevent.

| Choice | Why it is there |
| --- | --- |
| `tMin = 0.001` on bounce rays | After a hit, the origin sits on the surface. A `tMin` of 0 re-intersects the same face (shadow acne / black speckles). |
| Jittered `u,v` per sample | One ray through the pixel center aliases every edge. Random subpixel offsets turn aliasing into noise, which averages out. |
| Many samples per pixel | Path tracing is a Monte Carlo estimate. Noise is variance. More samples = cleaner image. Indoor GI needs more than a sky-lit field. |
| Per-sample radiance clamp (`8.0`) | A bounce that luckily hits a tiny bright light or the sun disc produces a firefly. Clamping is slightly biased but stops white sparkles. |
| Soft sky sun (wide disc, moderate energy) | A mathematically tiny sun is almost never sampled, then blows the pixel when it is. A wider, dimmer sun is stabler. |
| Environment modes (day / night / indoor black) | A Cornell box or metal dome should not leak a blue daytime sky through every miss. Indoor scenes use black; night uses a dark gradient. |
| `DiffuseLight` instead of shiny metal for lamps | Metal only reflects. If nothing in the scene **emits**, every path dies at black and the image is empty. “Bright metal” is not a light. |
| Russian roulette after a few bounces | Deep paths contribute almost nothing but cost full recursion. Randomly killing them (and weighting survivors by `1/p`) keeps the estimator honest while cutting work. |
| ACES + gamma 2.2 | Linear HDR averages are physically right but look washed or clipped on an sRGB display. Filmic mapping keeps highlights, gamma matches monitors. |
| `thread_local` RNGs in `Vec3` | One global `rand` from many threads races and correlates noise. Each worker has its own generator. |
| Materials stored in a `shared_ptr` vector | Spheres hold a raw `Material*`. The vector owns the memory for the whole render so those pointers stay valid. |
| Negative sphere radius | `outwardNormal = (point - center) / radius`. A negative radius flips the normal, which is how a hollow glass shell is built (outer `+1`, inner `-0.95`). |
| `abs(radius)` in the sphere AABB | A negative radius would invert `min`/`max` and break the BVH. The box always uses the geometric size. |
| BVH over a linear list | Scene 1 has ~80 small spheres. Naive `O(n)` hits per ray are fine for five objects and painful for hundreds. |
| Live GLFW preview, not a PPM write | You can tell in seconds if the camera or materials are wrong. Closing the window currently only stops the UI wait; worker threads still finish the frame. |
| GLFW vendored in `third_party/` | Clone and build without hunting a system package. Docs/tests/examples are turned off in CMake to keep the configure step short. |
| Scanline threading (not per-pixel) | Whole rows share cache-friendly loops and need only one atomic counter for the progress bar / texture upload. |

## Repository layout

```
src/                 tracer, camera, materials, window
third_party/glfw/    windowing + OpenGL context
CMakeLists.txt       C++17 executable `RayTracer`
```

### `src/` map

| File | Role |
| --- | --- |
| `main.cpp` | Scene graph, camera, sample/depth/environment per scene, window loop |
| `Renderer.cpp` | Integrator, threading, tone map, sky |
| `Camera.cpp` | Thin lens: origin on the aperture disk, rays through the focus plane |
| `Ray.h` | `P(t) = origin + t * direction` |
| `Math.h` | `Vec3`, random hemisphere/sphere, reflect, refract |
| `Hittable.h` | Hit + bounding-box interface |
| `HitRecord.h` | Intersection payload; `setFaceNormal` so dielectrics know inside vs outside |
| `HittableList.cpp` | List of objects; also used as the scene root that holds one BVH |
| `Sphere.cpp` | Analytic sphere; negative radius flips normals |
| `Triangle.cpp` | Möller–Trumbore triangles (used for flat walls if you add quads) |
| `AABB.h` / `BVH.cpp` | Axis-aligned boxes and a binary volume hierarchy |
| `Material.h` | Lambertian, Metal, Dielectric, DiffuseLight |
| `Image.cpp` | Float RGB buffer (PPM save still exists on the class, unused by `main`) |
| `Window.cpp` | GLFW window + `GL_TEXTURE_2D` blit of completed rows |

Read in that order if you are new: `Ray` → `Sphere::hit` → `Material::scatter` → `Renderer::rayColor` → `main` scenes.

## Build

Needs CMake 3.10+, a C++17 compiler, and OpenGL. GLFW is already in the tree.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

On macOS the OpenGL headers may warn that the API is deprecated. That is the system SDK, not a logic error.

## Run

```bash
./build/RayTracer      # scene 1
./build/RayTracer 2    # scene 2 … 5
```

Default resolution is **800×450**. Sample count and max depth are per scene (see below). Leave the window open until the progress line hits 100%.

Renders are not written to disk. If you want a file, call `image.savePPM(...)` after `renderer.render` in `main.cpp`.

## Scenes

All of these are built in `buildScene` in `src/main.cpp`.

### 1 — Classic sphere field (100 spp, depth 50)

Large grey ground, glass center, matte blue left, gold metal right, ~80 random mini spheres. Camera `(13, 2, 3)` looking at the origin, fov 20, aperture `0.1`, focus `10`. Daylight sky is the light source. This is the “cover image” stress test: DoF, mixed materials, BVH.

### 2 — Cornell box (200 spp, depth 50)

Red / green / white walls made from huge spheres, a small **emissive** ceiling light, a white sphere and a glass sphere on the floor. The front is open to the sky. 200 samples because enclosed bounce lighting is noisy. Objects sit on the floor (`y = radius`); burying them under `y = 0` would hide them inside the ground sphere.

### 3 — Night ring (150 spp, depth 50)

Dark ground, glowing gold center (`DiffuseLight`), eight orbiting glass/color spheres, a giant dark-blue metal sky shell. Environment is **black** so misses do not become daylight. The gold sphere has to emit: a metal ball in a closed metal shell with no lights is a black image.

### 4 — Hollow glass (200 spp, depth 50)

Dielectric radius `1.0` plus an inner dielectric radius `-0.95` (normal flip). That is a glass bubble, not a solid ball. Green ground, blue matte, fuzzy gold, small chrome floater. Sky-lit.

### 5 — Hall of mirrors (300 spp, depth 100)

Metal floor/ceiling and perfect silver side walls, red diffuse center, small glass and gold companions. Depth is 100 so mirror chains can run a long time before they are cut. Open ends still see the day sky, which is what lights the corridor.

## Materials (short)

- **Lambertian** — random bounce biased by the normal. Matte paint. `albedo` is the surface color.
- **Metal** — reflect about the normal; `fuzz` perturbs the direction (`0` = mirror).
- **Dielectric** — refract or reflect (total internal reflection + Schlick). Index `1.5` is glass, `1.33` is water. Attenuation is white (no absorption).
- **DiffuseLight** — `scatter` returns false; `emitted()` is the radiance. This is the only material that injects energy.

## Performance expectations

Path tracing cost is roughly `width × height × samples × average bounces`. Geometry is cheap until you have thousands of primitives; **samples and glass/mirrors** dominate.

Ballpark on a modern laptop, 800×450:

- Scene 1 (~100 spp): a couple of minutes
- Scene 2 / 4 (200 spp, GI or glass): longer
- Scene 5 (300 spp, depth 100): the slowest — every wall is a mirror

Use `Release`. Debug builds will feel an order of magnitude slower.

## Extending it

Useful next steps, in roughly increasing difficulty:

1. Raise samples / resolution in `main`.
2. Add a quad helper (`two triangles`) for a real Cornell box instead of giant spheres.
3. Next-event estimation (explicit light sampling) to kill indoor noise without 1000 spp.
4. Bounding boxes for every new shape; the BVH constructor prints if one is missing.
5. Importance-sampled GGX if you want roughness that is not “metal fuzz.”

Keep new shapes behind `Hittable` and new BRDFs behind `Material::scatter`. The renderer should not special-case object types.

## Credits

The structure follows Peter Shirley’s *Ray Tracing in One Weekend* series (rays, spheres, materials, BVH, Cornell-style lighting). GLFW is used only for the window and GL context.
