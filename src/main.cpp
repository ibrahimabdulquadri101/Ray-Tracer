#include "Renderer.h"
#include "Sphere.h"
#include "HittableList.h"
#include "BVH.h"
#include "Camera.h"
#include "Image.h"
#include "Material.h"
#include "Window.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>

HittableList buildScene(int scene_id, std::vector<std::shared_ptr<Material>>& materials,
                        Camera& camera, int& environment, int& samples, int& depth)
{
    HittableList world;
    const float pi = 3.14159265f;

    if (scene_id == 1) {
        std::cout << "Building Scene 1: Classic Sphere Field\n";
        environment = 0;
        samples = 100;
        depth = 50;

        materials.push_back(std::make_shared<Lambertian>(Vec3(0.5f, 0.5f, 0.5f)));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, -1000.0f, 0.0f), 1000.0f, materials[0].get()));

        materials.push_back(std::make_shared<Dielectric>(1.5f));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 1.0f, 0.0f), 1.0f, materials[1].get()));

        materials.push_back(std::make_shared<Lambertian>(Vec3(0.1f, 0.2f, 0.9f)));
        world.add(std::make_shared<Sphere>(Vec3(-4.0f, 1.0f, 0.0f), 1.0f, materials[2].get()));

        materials.push_back(std::make_shared<Metal>(Vec3(0.8f, 0.6f, 0.2f), 0.0f));
        world.add(std::make_shared<Sphere>(Vec3(4.0f, 1.0f, 0.0f), 1.0f, materials[3].get()));

        int placed = 0;
        for (int a = -11; a < 11 && placed < 80; ++a) {
            for (int b = -11; b < 11 && placed < 80; ++b) {
                float chooseMat = Vec3::randomFloat();
                Vec3 center(a + 0.9f * Vec3::randomFloat(), 0.2f, b + 0.9f * Vec3::randomFloat());

                if ((center - Vec3(0.0f, 0.2f, 0.0f)).length() < 0.9f) continue;
                if ((center - Vec3(-4.0f, 0.2f, 0.0f)).length() < 0.9f) continue;
                if ((center - Vec3(4.0f, 0.2f, 0.0f)).length() < 0.9f) continue;

                std::shared_ptr<Material> mat;
                if (chooseMat < 0.8f) {
                    Vec3 albedo = Vec3::random() * Vec3::random();
                    mat = std::make_shared<Lambertian>(albedo);
                } else if (chooseMat < 0.95f) {
                    Vec3 albedo = Vec3::random(0.5f, 1.0f);
                    float fuzz = Vec3::randomFloat(0.0f, 0.5f);
                    mat = std::make_shared<Metal>(albedo, fuzz);
                } else {
                    mat = std::make_shared<Dielectric>(1.5f);
                }

                materials.push_back(mat);
                world.add(std::make_shared<Sphere>(center, 0.2f, materials.back().get()));
                ++placed;
            }
        }

        camera.init(Vec3(13.0f, 2.0f, 3.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                    20.0f, 16.0f / 9.0f, 0.1f, 10.0f);

    } else if (scene_id == 2) {
        std::cout << "Building Scene 2: Cornell Box\n";
        environment = 0;
        samples = 200;
        depth = 50;

        materials.push_back(std::make_shared<Lambertian>(Vec3(0.65f, 0.05f, 0.05f)));
        materials.push_back(std::make_shared<Lambertian>(Vec3(0.12f, 0.45f, 0.15f)));
        materials.push_back(std::make_shared<Lambertian>(Vec3(0.73f, 0.73f, 0.73f)));
        materials.push_back(std::make_shared<DiffuseLight>(Vec3(12.0f, 12.0f, 12.0f)));
        materials.push_back(std::make_shared<Dielectric>(1.5f));

        world.add(std::make_shared<Sphere>(Vec3(-505.0f, 0.0f, 0.0f), 500.0f, materials[0].get()));
        world.add(std::make_shared<Sphere>(Vec3(505.0f, 0.0f, 0.0f), 500.0f, materials[1].get()));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, -500.0f, 0.0f), 500.0f, materials[2].get()));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 505.0f, 0.0f), 500.0f, materials[2].get()));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 0.0f, -505.0f), 500.0f, materials[2].get()));

        world.add(std::make_shared<Sphere>(Vec3(0.0f, 4.0f, -3.0f), 0.5f, materials[3].get()));
        // Spec listed y=-4, which sits inside the floor; rest these on the floor instead
        world.add(std::make_shared<Sphere>(Vec3(-1.0f, 1.0f, -3.0f), 1.0f, materials[2].get()));
        world.add(std::make_shared<Sphere>(Vec3(1.0f, 0.5f, -3.0f), 0.5f, materials[4].get()));

        camera.init(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                    40.0f, 16.0f / 9.0f, 0.0f, 5.0f);

    } else if (scene_id == 3) {
        std::cout << "Building Scene 3: Night Sky with Glowing Spheres\n";
        environment = 2;
        samples = 150;
        depth = 50;

        materials.push_back(std::make_shared<Lambertian>(Vec3(0.05f, 0.05f, 0.05f)));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, -1000.0f, 0.0f), 1000.0f, materials[0].get()));

        materials.push_back(std::make_shared<DiffuseLight>(Vec3(8.0f, 6.8f, 2.4f)));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 1.0f, 0.0f), 1.0f, materials[1].get()));

        Vec3 orbitColors[] = {
            Vec3(0.95f, 0.15f, 0.15f),
            Vec3(0.15f, 0.85f, 0.25f),
            Vec3(0.15f, 0.35f, 0.95f),
            Vec3(0.95f, 0.75f, 0.15f)
        };
        materials.push_back(std::make_shared<Dielectric>(1.5f));
        for (int i = 0; i < 4; ++i) {
            materials.push_back(std::make_shared<Lambertian>(orbitColors[i]));
        }

        for (int i = 0; i < 8; ++i) {
            float ang = static_cast<float>(i) * 2.0f * pi / 8.0f;
            Vec3 pos(3.0f * std::cos(ang), 0.3f, 3.0f * std::sin(ang));
            if (i % 2 == 0) {
                world.add(std::make_shared<Sphere>(pos, 0.3f, materials[2].get()));
            } else {
                world.add(std::make_shared<Sphere>(pos, 0.3f, materials[3 + (i / 2)].get()));
            }
        }

        materials.push_back(std::make_shared<Metal>(Vec3(0.02f, 0.02f, 0.1f), 0.0f));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 0.0f, 0.0f), 500.0f, materials.back().get()));

        camera.init(Vec3(8.0f, 3.0f, 8.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                    30.0f, 16.0f / 9.0f, 0.05f, 10.0f);

    } else if (scene_id == 4) {
        std::cout << "Building Scene 4: Hollow Glass Sphere\n";
        environment = 0;
        samples = 200;
        depth = 50;

        materials.push_back(std::make_shared<Lambertian>(Vec3(0.2f, 0.8f, 0.2f)));
        materials.push_back(std::make_shared<Dielectric>(1.5f));
        materials.push_back(std::make_shared<Lambertian>(Vec3(0.1f, 0.2f, 0.9f)));
        materials.push_back(std::make_shared<Metal>(Vec3(0.8f, 0.6f, 0.2f), 0.1f));
        materials.push_back(std::make_shared<Metal>(Vec3(0.9f, 0.9f, 0.9f), 0.0f));

        world.add(std::make_shared<Sphere>(Vec3(0.0f, -100.0f, 0.0f), 100.0f, materials[0].get()));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 1.0f, 0.0f), 1.0f, materials[1].get()));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 1.0f, 0.0f), -0.95f, materials[1].get()));
        world.add(std::make_shared<Sphere>(Vec3(-2.5f, 0.5f, 0.0f), 0.5f, materials[2].get()));
        world.add(std::make_shared<Sphere>(Vec3(2.5f, 0.5f, 0.0f), 0.5f, materials[3].get()));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 2.5f, 0.0f), 0.3f, materials[4].get()));

        camera.init(Vec3(5.0f, 2.0f, 5.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                    35.0f, 16.0f / 9.0f, 0.0f, 7.0f);

    } else {
        std::cout << "Building Scene 5: Metal Hall of Mirrors\n";
        environment = 0;
        samples = 300;
        depth = 100;

        materials.push_back(std::make_shared<Metal>(Vec3(0.7f, 0.7f, 0.7f), 0.05f));
        materials.push_back(std::make_shared<Metal>(Vec3(0.9f, 0.9f, 0.95f), 0.0f));
        materials.push_back(std::make_shared<Lambertian>(Vec3(0.9f, 0.1f, 0.1f)));
        materials.push_back(std::make_shared<Dielectric>(1.5f));
        materials.push_back(std::make_shared<Metal>(Vec3(1.0f, 0.85f, 0.2f), 0.0f));

        world.add(std::make_shared<Sphere>(Vec3(0.0f, -1001.0f, 0.0f), 1000.0f, materials[0].get()));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 1005.0f, 0.0f), 1000.0f, materials[0].get()));
        world.add(std::make_shared<Sphere>(Vec3(-1005.0f, 0.0f, 0.0f), 1000.0f, materials[1].get()));
        world.add(std::make_shared<Sphere>(Vec3(1005.0f, 0.0f, 0.0f), 1000.0f, materials[1].get()));
        world.add(std::make_shared<Sphere>(Vec3(0.0f, 0.0f, 0.0f), 1.0f, materials[2].get()));
        world.add(std::make_shared<Sphere>(Vec3(1.5f, 0.0f, -1.0f), 0.4f, materials[3].get()));
        world.add(std::make_shared<Sphere>(Vec3(-1.5f, 0.0f, -1.0f), 0.4f, materials[4].get()));

        camera.init(Vec3(0.0f, 0.0f, 6.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                    45.0f, 16.0f / 9.0f, 0.0f, 6.0f);
    }

    std::vector<std::shared_ptr<Hittable>> objects = world.getObjects();
    auto bvh = std::make_shared<BVHNode>(objects, 0, objects.size(), 0.0f, 1.0f);

    HittableList scene;
    scene.add(bvh);
    return scene;
}

int main(int argc, char* argv[])
{
    int scene_id = 1;
    if (argc > 1) {
        scene_id = std::atoi(argv[1]);
    }

    std::vector<std::shared_ptr<Material>> materials;
    Camera camera;
    int environment = 0;
    int samples = 100;
    int depth = 50;
    HittableList scene = buildScene(scene_id, materials, camera, environment, samples, depth);

    int width = 800;
    int height = 450;

    Window window;
    if (window.init(width, height, "Ray Tracer - Scene " + std::to_string(scene_id))) {
        std::cout << "GLFW Window initialized successfully.\n";
    }

    Renderer renderer;
    renderer.init(width, height, samples, depth);
    renderer.setEnvironment(environment);

    Image image;
    std::cout << "Rendering " << width << "x" << height << " with "
              << samples << " samples/pixel, max depth " << depth << "...\n";

    renderer.render(scene, camera, image, &window);

    while (!window.shouldClose()) {
        window.display();
        window.pollEvents();
    }

    return 0;
}
