#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"
#include "HitRecord.h"

// Forward declaration — AABB will be defined in BVH.h
class AABB;

class Hittable {
public:
    virtual ~Hittable() = default;

    // Returns true if the ray hits this object within [tMin, tMax].
    // tMin — prevents self-intersection (shadow acne): rays that just bounced off
    //         a surface would immediately re-hit it at t≈0 without this lower bound.
    // tMax — lets the caller progressively shrink the search window as closer hits
    //         are found, so only the nearest intersection is kept.
    // rec  — filled with hit details (point, normal, material, t) if a hit occurs.
    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const = 0;

    // Returns the axis-aligned bounding box of this object over time [t0, t1].
    // Required by the BVH to build the acceleration structure.
    virtual bool boundingBox(float t0, float t1, AABB& box) const { return false; }
};

#endif // HITTABLE_H
