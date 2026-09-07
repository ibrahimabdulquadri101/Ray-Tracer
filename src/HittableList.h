#pragma once
#include "Hittable.h"
#include <vector>
#include <memory>

class HittableList : public Hittable {
public:
    HittableList();
    
    void clear();
    void add(std::shared_ptr<Hittable> object);
    
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
    
private:
    std::vector<std::shared_ptr<Hittable>> objects;
};
