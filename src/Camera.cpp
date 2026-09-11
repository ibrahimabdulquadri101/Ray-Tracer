#include "Camera.h"
#include "Ray.h"
#include <cmath>

void Camera::init(Vec3 lookFrom, Vec3 lookAt, Vec3 up, float vfov, float aspectRatio, float aperture, float focusDistance)
{
    float theta = vfov * (M_PI / 180.0f);
    float h = std::tan(theta / 2.0f);
    float hViewport = 2.0 * h;
    float wViewport = aspectRatio * hViewport;
    w = (lookFrom - lookAt).normalize();
    u = up.cross(w).normalize();
    v = w.cross(u);
    origin = lookFrom;
    horizontal = focusDistance * wViewport * u;
    vertical = focusDistance * hViewport * v;
    lowerLeftCorner = origin - horizontal/2 - vertical/2 - focusDistance * w;
    lensRadius = aperture / 2;
}

Ray Camera::getRay(float s, float t)
{
    Vec3 offset(0.0f, 0.0f, 0.0f);
    
    // If aperture is greater than 0, simulate depth of field (blur)
    if (lensRadius > 0.0f) {
        Vec3 rd;
        // Generate a random point inside a 2D unit disk
        while (true) {
            rd = Vec3::random(-1.0f, 1.0f);
            rd.z = 0.0f; // Flatten the sphere to a disk
            if (rd.dot(rd) < 1.0f) break;
        }
        rd = rd * lensRadius;
        offset = (u * rd.x) + (v * rd.y);
    }

    // Calculate final ray direction from the (possibly offset) origin
    Vec3 direction = lowerLeftCorner + (horizontal * s) + (vertical * t) - origin - offset;
    
    return Ray(origin + offset, direction);
}