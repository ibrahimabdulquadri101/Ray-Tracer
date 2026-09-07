#pragma once
#include "Math.h"

class Ray {
public:
    Ray();
    Ray(const Vec3& origin, const Vec3& direction);
    
    Vec3 getOrigin() const;
    Vec3 getDirection() const;
    Vec3 at(float t) const;
    
private:
    Vec3 origin;
    Vec3 direction;
};
