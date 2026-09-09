#include "Sphere.h"
#include "Ray.h"
#include "HitRecord.h"
#include <iostream>

int main()
{
    // Create a sphere at the origin with radius 1, no material needed for this test
    Sphere sphere(Vec3(0.0f, 0.0f, 0.0f), 1.0f, nullptr);

    // Shoot a ray from (0, 0, -5) toward (0, 0, 1) — pointing at the sphere
    Ray ray(Vec3(0.0f, 0.0f, -5.0f), Vec3(0.0f, 0.0f, 1.0f));

    HitRecord rec;
    bool hit = sphere.hit(ray, 0.001f, 1e9f, rec);

    if (hit) {
        std::cout << "Hit!" << std::endl;
        std::cout << "t         = " << rec.t << "  (expected ~4.0)" << std::endl;
        std::cout << "Hit point = ("
                  << rec.point.x << ", "
                  << rec.point.y << ", "
                  << rec.point.z << ")  (expected ~(0, 0, -1))" << std::endl;
        std::cout << "Normal    = ("
                  << rec.normal.x << ", "
                  << rec.normal.y << ", "
                  << rec.normal.z << ")  (expected ~(0, 0, -1))" << std::endl;
        std::cout << "Front face: " << (rec.frontFace ? "true" : "false")
                  << "  (expected true)" << std::endl;
    } else {
        std::cout << "No hit — something is wrong." << std::endl;
    }

    return 0;
}
