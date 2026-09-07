#pragma once
#include "Hittable.h"
#include "Math.h"
#include <memory>

class Material;

class Triangle : public Hittable {
public:
    Triangle();
    Triangle(Vec3 v0, Vec3 v1, Vec3 v2, std::shared_ptr<Material> m);
    
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;

private:
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    std::shared_ptr<Material> mat;
};
