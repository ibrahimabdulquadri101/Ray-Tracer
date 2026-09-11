#include "Image.h"
#include <fstream>
#include <iostream>

void Image::init(int w, int h)
{
    width = w;
    height = h;
    pixels.resize(w * h);
}

void Image::setPixel(int x, int y, Vec3 color)
{
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixels[y * width + x] = color;
    }
}

bool Image::savePPM(const std::string& filename)
{
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error opening file for writing: " << filename << "\n";
        return false;
    }

    // P3 ASCII PPM header
    out << "P3\n" << width << " " << height << "\n255\n";

    // Write pixels top to bottom
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vec3 color = pixels[y * width + x];

            // Clamp to [0, 1], scale to [0, 255], round to int
            int ir = static_cast<int>(255.99f * clamp(color.x, 0.0f, 0.999f));
            int ig = static_cast<int>(255.99f * clamp(color.y, 0.0f, 0.999f));
            int ib = static_cast<int>(255.99f * clamp(color.z, 0.0f, 0.999f));

            out << ir << " " << ig << " " << ib << "\n";
        }
    }

    out.close();
    return true;
}