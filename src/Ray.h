#ifndef RAY_H
#define RAY_H

#include "Math.h"

struct Ray
{
    Vec3 origin;
    Vec3 direction;

    Ray() {}

    Ray(const Vec3& origin, const Vec3& direction) {
        this->origin    = origin;
        this->direction = direction;
    }

    // Returns the point along the ray at parameter t: P(t) = origin + t*direction
    Vec3 at(float t) const {
        return origin + (direction * t);
    }
};

#endif // RAY_H
