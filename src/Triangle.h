#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "Math.h"
#include "Material.h"
#include "Hittable.h"
class Triangle : public Hittable
{
    public:
        Vec3 v0,v1,v2;
        Vec3 normal;
        Material* material;

        Triangle() {}
        Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, Material* material)
            : v0(v0), v1(v1), v2(v2), material(material)
        {
            // Compute face normal automatically using cross product and normalize
            Vec3 e1 = v1 - v0;
            Vec3 e2 = v2 - v0;
            normal = cross(e1, e2).normalize();
        }

        virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const override;
        virtual bool boundingBox(float t0, float t1, AABB& box) const override;
};

#endif // TRIANGLE_H
