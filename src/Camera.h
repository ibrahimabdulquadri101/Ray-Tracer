#pragma once
#include "Math.h"
#include "Ray.h"

class Camera {
public:
    Camera();
    Camera(Vec3 lookfrom, Vec3 lookat, Vec3 vup, float vfov, float aspect_ratio);
    
    Ray get_ray(float s, float t) const;

private:
    Vec3 origin;
    Vec3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
};
