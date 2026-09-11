#ifndef AABB_H
#define AABB_H

#include "Math.h"
#include "Ray.h"
#include <algorithm>

struct AABB
{
    Vec3 minimum;
    Vec3 maximum;

    AABB() {}
    AABB(const Vec3& min, const Vec3& max) : minimum(min), maximum(max) {}

    Vec3 min() const { return minimum; }
    Vec3 max() const { return maximum; }

    bool hit(const Ray& ray, float tMin, float tMax) const
    {
        // X axis
        {
            float invD = 1.0f / ray.direction.x;
            float t0 = (minimum.x - ray.origin.x) * invD;
            float t1 = (maximum.x - ray.origin.x) * invD;
            if (invD < 0.0f) std::swap(t0, t1);
            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax <= tMin) return false;
        }
        // Y axis
        {
            float invD = 1.0f / ray.direction.y;
            float t0 = (minimum.y - ray.origin.y) * invD;
            float t1 = (maximum.y - ray.origin.y) * invD;
            if (invD < 0.0f) std::swap(t0, t1);
            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax <= tMin) return false;
        }
        // Z axis
        {
            float invD = 1.0f / ray.direction.z;
            float t0 = (minimum.z - ray.origin.z) * invD;
            float t1 = (maximum.z - ray.origin.z) * invD;
            if (invD < 0.0f) std::swap(t0, t1);
            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax <= tMin) return false;
        }
        return true;
    }
};

inline AABB surroundingBox(const AABB& box0, const AABB& box1)
{
    Vec3 small(std::fmin(box0.min().x, box1.min().x),
               std::fmin(box0.min().y, box1.min().y),
               std::fmin(box0.min().z, box1.min().z));
    Vec3 big(std::fmax(box0.max().x, box1.max().x),
             std::fmax(box0.max().y, box1.max().y),
             std::fmax(box0.max().z, box1.max().z));
    return AABB(small, big);
}

#endif // AABB_H
