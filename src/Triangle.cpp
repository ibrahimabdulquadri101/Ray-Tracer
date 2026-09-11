#include "Triangle.h"
#include <cmath>

bool Triangle::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const
{
    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;
    Vec3 h = ray.direction.cross(e2);
    float a = e1.dot(h);
    
    // Check if ray is parallel to the triangle
    if (std::abs(a) < 1e-8f)
    {
        return false;
    }

    float f = 1.0f / a;
    Vec3 s = ray.origin - v0;
    float u = f * s.dot(h);
    
    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }

    Vec3 q = s.cross(e1);
    float v = f * ray.direction.dot(q);
    
    if (v < 0.0f || (u + v) > 1.0f)
    {
        return false;
    }

    float t = f * e2.dot(q);
    
    if (t < tMin || t > tMax)
    {
        return false;
    }

    rec.t = t;
    rec.point = ray.at(rec.t);
    rec.setFaceNormal(ray, normal);
    rec.material = material;

    return true;
}

bool Triangle::boundingBox(float t0, float t1, AABB& box) const
{
    (void)t0;
    (void)t1;
    Vec3 small(std::fmin(v0.x, std::fmin(v1.x, v2.x)),
               std::fmin(v0.y, std::fmin(v1.y, v2.y)),
               std::fmin(v0.z, std::fmin(v1.z, v2.z)));
    Vec3 big(std::fmax(v0.x, std::fmax(v1.x, v2.x)),
             std::fmax(v0.y, std::fmax(v1.y, v2.y)),
             std::fmax(v0.z, std::fmax(v1.z, v2.z)));
    const float pad = 0.0001f;
    box = AABB(small - Vec3(pad, pad, pad), big + Vec3(pad, pad, pad));
    return true;
}