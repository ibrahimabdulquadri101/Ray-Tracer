#pragma once
#include "Ray.h"
#include "HitRecord.h"

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
};
