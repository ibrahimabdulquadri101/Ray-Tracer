#pragma once
#include "Hittable.h"
#include "Math.h"
#include <memory>

class Material;

class Sphere : public Hittable {
public:
    Sphere();
    Sphere(Vec3 center, float radius, std::shared_ptr<Material> m);
    
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;

private:
    Vec3 center;
    float radius;
    std::shared_ptr<Material> mat;
};
