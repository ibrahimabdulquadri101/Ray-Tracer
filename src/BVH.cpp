#include "BVH.h"
#include <algorithm>

inline bool boxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis) 
{
    AABB boxA;
    AABB boxB;

    if (!a->boundingBox(0, 0, boxA) || !b->boundingBox(0, 0, boxB))
        std::cerr << "No bounding box in BVHNode constructor.\n";

    float minA = (axis == 0) ? boxA.min().x : (axis == 1) ? boxA.min().y : boxA.min().z;
    float minB = (axis == 0) ? boxB.min().x : (axis == 1) ? boxB.min().y : boxB.min().z;
    return minA < minB;
}

inline bool boxXCompare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) 
{
    return boxCompare(a, b, 0);
}
inline bool boxYCompare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) 
{
    return boxCompare(a, b, 1);
}
inline bool boxZCompare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) 
{
    return boxCompare(a, b, 2);
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end, float t0, float t1)
{
    int axis = (int)(Vec3::randomFloat(0.0f, 3.0f));
    auto comparator = (axis == 0) ? boxXCompare
                    : (axis == 1) ? boxYCompare
                    : boxZCompare;

    size_t objectSpan = end - start;

    if (objectSpan == 1) {
        left = right = objects[start];
    } else if (objectSpan == 2) {
        if (comparator(objects[start], objects[start+1])) {
            left = objects[start];
            right = objects[start+1];
        } else {
            left = objects[start+1];
            right = objects[start];
        }
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        size_t mid = start + objectSpan / 2;
        left = std::make_shared<BVHNode>(objects, start, mid, t0, t1);
        right = std::make_shared<BVHNode>(objects, mid, end, t0, t1);
    }

    AABB boxLeft, boxRight;

    if (!left->boundingBox(t0, t1, boxLeft) || !right->boundingBox(t0, t1, boxRight))
        std::cerr << "No bounding box in BVHNode constructor.\n";

    box = surroundingBox(boxLeft, boxRight);
}

bool BVHNode::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const
{
    if (!box.hit(ray, tMin, tMax))
        return false;

    bool hitLeft = left->hit(ray, tMin, tMax, rec);
    bool hitRight = right->hit(ray, tMin, hitLeft ? rec.t : tMax, rec);

    return hitLeft || hitRight;
}

bool BVHNode::boundingBox(float t0, float t1, AABB& outputBox) const
{
    outputBox = box;
    return true;
}
