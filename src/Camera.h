#ifndef CAMERA_H
#define CAMERA_H
#include "Math.h"
#include "Ray.h"

class Camera
{
    private:
        Vec3 origin;
        Vec3 lowerLeftCorner;
        Vec3 horizontal;
        Vec3 vertical;
        Vec3 u,v,w;
        float lensRadius;
    public:
        void init(Vec3 lookFrom , Vec3 lookAt,Vec3 up , float vfov,float aspectRatio, float aperture,float focusDistance);
        Ray getRay(float s , float t);
};

#endif // CAMERA_H
