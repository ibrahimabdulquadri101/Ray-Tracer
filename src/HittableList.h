#ifndef HITTABLELIST_H
#define HITTABLELIST_H
#include <vector>
#include "Hittable.h"
#include <memory>

class HittableList : public Hittable
{
    private:
        std::vector<std::shared_ptr<Hittable>> objects;
    public:
        void add(std::shared_ptr<Hittable> object);
        void clear();
        bool hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const override;
        bool boundingBox(float t0, float t1, AABB& box) const override;
        const std::vector<std::shared_ptr<Hittable>>& getObjects() const { return objects; }
};

#endif // HITTABLELIST_H
