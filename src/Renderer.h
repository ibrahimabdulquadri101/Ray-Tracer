#ifndef RENDERER_H
#define RENDERER_H
#include "HittableList.h"
#include "Camera.h"
#include "Image.h"

class Renderer
{
    private:
        int imageWidth;
        int imageHeight;
        int samplesPerPixel;
        int maxDepth;
        int environmentMode; // 0 daylight, 1 night/space, 2 indoor black
    public:
        void init(int width, int height, int samples, int depth);
        void setEnvironment(int mode);
        void render(HittableList& scene, Camera& camera, Image& image, class Window* window = nullptr);
        Vec3 rayColor(Ray& ray, HittableList& scene, int depth);
        Vec3 toneMap(Vec3 color);
};

#endif // RENDERER_H
