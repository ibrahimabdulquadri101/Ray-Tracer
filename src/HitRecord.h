#pragma once
#include "Math.h"

class Material;
class Ray;

struct HitRecord {
    Vec3 p;
    Vec3 normal;
    Material* material;
    float t;
    bool front_face;
    
    void set_face_normal(const Ray& r, const Vec3& outward_normal);
};
