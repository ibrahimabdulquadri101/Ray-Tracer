#include "Math.h"
#include "Hittable.h"
#include "Renderer.h"
#include "HitRecord.h"
#include "Material.h"
#include "Window.h"
#include <limits>
#include <iostream>
#include <cmath>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

float FLT_MAX = std::numeric_limits<float>::max();

namespace {
    // Narkowicz 2015 — "ACES Filmic Tone Mapping Curve"
    // Maps HDR radiance to [0,1] with a soft shoulder, keeping highlights
    // from clipping to pure white the way a raw linear clamp does.
    Vec3 acesFilmic(Vec3 x)
    {
        const float a = 2.51f, b = 0.03f, c = 2.43f, d = 0.59f, e = 0.14f;
        Vec3 t = (x * (a * x + Vec3(b, b, b))) / (x * (c * x + Vec3(d, d, d)) + Vec3(e, e, e));
        return Vec3(clamp(t.x, 0.0f, 1.0f),
                    clamp(t.y, 0.0f, 1.0f),
                    clamp(t.z, 0.0f, 1.0f));
    }
}

void Renderer::init(int width, int height, int samples, int depth)
{
    imageWidth = width;
    imageHeight = height;
    samplesPerPixel = samples;
    maxDepth = depth;
    environmentMode = 0;
}

void Renderer::setEnvironment(int mode)
{
    environmentMode = mode;
}

void Renderer::render(HittableList& scene, Camera& camera, Image& image, Window* window)
{
    image.init(imageWidth, imageHeight);

    std::atomic<int> rowsDone{0};
    int numThreads = std::max(1, static_cast<int>(std::thread::hardware_concurrency()));
    std::vector<std::thread> workers;

    auto worker = [&](int startRow, int endRow)
    {
        for (int j = startRow; j < endRow; ++j)
        {
            for (int i = 0; i < imageWidth; ++i)
            {
                Vec3 pixelColor(0.0f, 0.0f, 0.0f);
                for (int s = 0; s < samplesPerPixel; ++s)
                {
                    float u = (i + Vec3::randomFloat()) / imageWidth;
                    float v = ((imageHeight - 1 - j) + Vec3::randomFloat()) / imageHeight;
                    Ray r = camera.getRay(u, v);
                    Vec3 sample = rayColor(r, scene, maxDepth);
                    // Per-sample clamp kills fireflies from tiny lights / the sun disc
                    const float maxRadiance = 8.0f;
                    sample = Vec3(std::fmin(sample.x, maxRadiance),
                                  std::fmin(sample.y, maxRadiance),
                                  std::fmin(sample.z, maxRadiance));
                    pixelColor = pixelColor + sample;
                }

                // Average the color by samples per pixel
                pixelColor = pixelColor / samplesPerPixel;

                // Filmic tone map + gamma 2.2
                pixelColor = toneMap(pixelColor);

                image.setPixel(i, j, pixelColor);
            }
            rowsDone.fetch_add(1);
        }
    };

    // Split scanlines evenly across the cores
    for (int t = 0; t < numThreads; ++t)
    {
        int start = (t * imageHeight) / numThreads;
        int end   = ((t + 1) * imageHeight) / numThreads;
        if (start < end)
            workers.emplace_back(worker, start, end);
    }

    while (rowsDone.load() < imageHeight)
    {
        if (window) {
            window->updateTexture(image.getPixels(), rowsDone.load());
            window->display();
            window->pollEvents();
            
            if (window->shouldClose()) {
                // If user closes window, we should really stop the threads.
                // But for now, we just stop waiting and let them finish or exit.
                // We'll let it finish in background.
                break;
            }
        }
        
        std::cout << "\rProgress: " << (int)(100.0f * rowsDone.load() / imageHeight)
                  << "% (" << numThreads << " threads) " << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    for (auto& t : workers) t.join();

    if (window) {
        window->updateTexture(image.getPixels(), imageHeight);
        window->display();
        window->pollEvents();
    }

    std::cout << "\rProgress: 100% - Render complete!          \n";
}

Vec3 Renderer::toneMap(Vec3 color)
{
    color = acesFilmic(color);
    const float invGamma = 1.0f / 2.2f;
    return Vec3(std::pow(color.x, invGamma),
                std::pow(color.y, invGamma),
                std::pow(color.z, invGamma));
}

Vec3 Renderer::rayColor(Ray& ray, HittableList& scene, int depth)
{
    if(depth <= 0)
    {
        return Vec3(0.0f,0.0f,0.0f);
    }
    HitRecord rec;
    Vec3 attenuation;
    Ray scattered;
    bool isHit = scene.hit(ray,0.001,FLT_MAX,rec);
    if(isHit)
    {
        // Start with any light this surface emits
        Vec3 color = rec.material->emitted();

        bool isScattered = rec.material->scatter(ray,rec,attenuation,scattered);
        if(isScattered)
        {
            // Russian roulette: deep, dim paths are stochastically terminated,
            // which keeps the estimator unbiased while removing nearly-invisible bounces
            if (depth < 4 || attenuation.length() <= 0.0f)
            {
                color = color + attenuation * rayColor(scattered, scene, depth-1);
            }
            else
            {
                float p = std::fmax(attenuation.x, std::fmax(attenuation.y, attenuation.z));
                if (Vec3::randomFloat() < p)
                {
                    color = color + (1.0f / p) * attenuation * rayColor(scattered, scene, depth-1);
                }
            }
        }
        return color;
    }else
    {
        if (environmentMode == 2) {
            return Vec3(0.0f, 0.0f, 0.0f);
        }

        Vec3 unitDir = ray.direction.normalize();
        float t = 0.5f * (unitDir.y + 1.0f);

        if (environmentMode == 1) {
            Vec3 zenith(0.004f, 0.008f, 0.02f);
            Vec3 horizon(0.02f, 0.03f, 0.05f);
            Vec3 sky = (1.0f - t) * horizon + t * zenith;

            Vec3 moonDir = Vec3(-0.35f, 0.55f, -0.4f).normalize();
            float moonDot = std::fmax(dot(unitDir, moonDir), 0.0f);
            float moonDisc = std::pow(moonDot, 96.0f);
            float moonHalo = std::pow(moonDot, 6.0f) * 0.15f;
            sky = sky + Vec3(1.6f, 1.7f, 2.0f) * moonDisc + Vec3(0.15f, 0.18f, 0.28f) * moonHalo;
            return sky;
        }

        Vec3 sunDir = Vec3(0.4f, 0.75f, -0.3f).normalize();
        float sunDot = std::fmax(dot(unitDir, sunDir), 0.0f);

        // Wide, moderate sun — a razor disc is a firefly factory
        float sunDisc = std::pow(sunDot, 48.0f) * 1.8f;
        float sunHalo = std::pow(sunDot, 6.0f) * 0.35f;

        Vec3 zenith(0.22f, 0.42f, 0.78f);
        Vec3 horizon(0.82f, 0.88f, 0.95f);
        Vec3 groundGlow(0.55f, 0.42f, 0.28f);

        Vec3 sky;
        if (unitDir.y >= 0.0f) {
            sky = (1.0f - t) * horizon + t * zenith;
        } else {
            float u = std::fmin(-unitDir.y * 4.0f, 1.0f);
            sky = (1.0f - u) * horizon + u * groundGlow;
        }

        sky = sky + Vec3(1.8f, 1.55f, 1.1f) * sunDisc + Vec3(1.0f, 0.85f, 0.6f) * sunHalo;
        return sky;
    }
}
