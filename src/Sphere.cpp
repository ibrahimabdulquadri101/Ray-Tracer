#include "Sphere.h"
#include <cmath>

Sphere::Sphere(Vec3 center, float radius, Material* material)
    : center(center), radius(radius), material(material) {}

bool Sphere::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const
{
    Vec3  oc     = ray.origin - center;
    float a      = ray.direction.dot(ray.direction);
    float halfB  = oc.dot(ray.direction);
    float c      = oc.dot(oc) - radius * radius;

    float discriminant = halfB * halfB - a * c;
    if (discriminant < 0) return false;

    float sqrtD = std::sqrt(discriminant);

    // Try the nearer root first
    float root = (-halfB - sqrtD) / a;
    if (root < tMin || root > tMax) {
        // Near root out of range, try the far root
        root = (-halfB + sqrtD) / a;
        if (root < tMin || root > tMax)
            return false;
    }

    rec.t        = root;
    rec.point    = ray.at(rec.t);
    Vec3 outwardNormal = (rec.point - center) / radius;
    rec.setFaceNormal(ray, outwardNormal);
    rec.material = material;

    return true;
}

bool Sphere::boundingBox(float t0, float t1, AABB& box) const
{
    float r = std::fabs(radius);
    box = AABB(center - Vec3(r, r, r),
               center + Vec3(r, r, r));
    return true;
}
