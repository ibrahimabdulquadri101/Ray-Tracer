#ifndef BVH_H
#define BVH_H

#include "Math.h"
#include "Ray.h"
#include "Hittable.h"
#include "AABB.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

class BVHNode : public Hittable
{
public:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;

    BVHNode() {}

    BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end, float t0, float t1);

    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const override;
    virtual bool boundingBox(float t0, float t1, AABB& outputBox) const override;
};

#endif // BVH_H
