#pragma once
#include "Math.h"
#include "Ray.h"
#include "HitRecord.h"

class Material {
public:
    virtual ~Material() = default;
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const = 0;
};
