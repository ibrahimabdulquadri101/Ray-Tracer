#pragma once
#include "Camera.h"
#include "HittableList.h"
#include "Image.h"

class Renderer {
public:
    Renderer(int width, int height);
    
    void render(const Camera& cam, const HittableList& world, Image& image);

private:
    int width;
    int height;
    Vec3 ray_color(const Ray& r, const Hittable& world, int depth);
};
