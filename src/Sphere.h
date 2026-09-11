#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include "Material.h"

class Sphere : public Hittable {
public:
    Vec3 center;
    float radius;
    Material* material;

    Sphere() {}
    Sphere(Vec3 center, float radius, Material* material);

    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const override;
    virtual bool boundingBox(float t0, float t1, AABB& box) const override;
};

#endif // SPHERE_H
