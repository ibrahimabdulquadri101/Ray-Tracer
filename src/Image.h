#pragma once
#include "Math.h"
#include <string>
#include <vector>

class Image {
public:
    Image(int width, int height);
    
    void setPixel(int x, int y, const Vec3& color);
    void save(const std::string& filename) const;
    
    int getWidth() const;
    int getHeight() const;

private:
    int width;
    int height;
    std::vector<Vec3> pixels;
};
