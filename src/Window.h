#ifndef WINDOW_H
#define WINDOW_H

#include "Math.h"
#include <vector>
#include <string>
#include <GLFW/glfw3.h>

class Window {
private:
    GLFWwindow* window;
    unsigned int textureID;
    int width;
    int height;

public:
    Window();
    ~Window();

    bool init(int w, int h, const std::string& title);
    void updateTexture(const std::vector<Vec3>& pixels, int rowsCompleted);
    void display();
    bool shouldClose();
    void pollEvents();
};

#endif // WINDOW_H
