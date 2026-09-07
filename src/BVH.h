#pragma once
#include "Hittable.h"
#include "HittableList.h"
#include <memory>

class BVHNode : public Hittable {
public:
    BVHNode();
    BVHNode(const HittableList& list, float time0, float time1);
    
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
    
private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
};
