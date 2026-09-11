#include "HittableList.h"
#include "BVH.h"
#include <vector>

void HittableList::add(std::shared_ptr<Hittable> object)
{
    objects.push_back(object);
}

void HittableList::clear()
{
    objects.clear();
}

bool HittableList::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const
{
    HitRecord tempRec;
    bool hitAnything = false;
    float closestSoFar = tMax;
    for(auto& object : objects)
    {
        if(object->hit(ray, tMin, closestSoFar, tempRec))
        {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }
    return hitAnything;
}

bool HittableList::boundingBox(float t0, float t1, AABB& box) const
{
    if (objects.empty()) return false;

    AABB tempBox;
    bool firstBox = true;
    for (const auto& object : objects) {
        if (!object->boundingBox(t0, t1, tempBox)) return false;
        box = firstBox ? tempBox : surroundingBox(box, tempBox);
        firstBox = false;
    }
    return true;
}