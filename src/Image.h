#ifndef IMAGE_H
#define IMAGE_H

#include "Math.h"
#include <vector>
#include <string>

class Image
{
private:
    int width;
    int height;
    std::vector<Vec3> pixels;

public:
    Image() : width(0), height(0) {}
    Image(int w, int h) : width(w), height(h), pixels(w * h) {}

    void init(int width, int height);
    void setPixel(int x, int y, Vec3 color);
    bool savePPM(const std::string& filename);
    const std::vector<Vec3>& getPixels() const { return pixels; }
};

#endif // IMAGE_H