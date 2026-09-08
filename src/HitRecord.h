#ifndef HITRECORD_H
#define HITRECORD_H

#include "Math.h"
#include "Ray.h"

// Forward declaration — breaks the circular include with Material.h
// (Material.h includes HitRecord.h, so HitRecord.h must NOT include Material.h)
class Material;

struct HitRecord {
    Vec3 point;
    Vec3 normal;
    float t;
    bool frontFace;
    Material* material;

    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = dot(ray.direction, outwardNormal) < 0;
        normal    = frontFace ? outwardNormal : -1.0f * outwardNormal;
    }
};

#endif // HITRECORD_H
