#include "Window.h"
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

Window::Window() : window(nullptr), textureID(0), width(0), height(0) {}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Window::init(int w, int h, const std::string& title) {
    width = w;
    height = h;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    // Use legacy OpenGL profile since we just want to draw a simple texture quad
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // Setup OpenGL texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Basic texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Allocate texture memory (rgb float)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

    return true;
}

void Window::updateTexture(const std::vector<Vec3>& pixels, int rowsCompleted) {
    if (!window || rowsCompleted <= 0) return;

    glBindTexture(GL_TEXTURE_2D, textureID);
    // Our pixels vector contains Vec3 (which is tightly packed 3 floats).
    // We upload the entire image. Unfinished rows will be black (default init).
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, pixels.data());
}

void Window::display() {
    if (!window) return;

    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
    // Top-left
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
    // Bottom-left
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
    // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
    // Top-right
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glfwSwapBuffers(window);
}

bool Window::shouldClose() {
    return window ? glfwWindowShouldClose(window) : true;
}

void Window::pollEvents() {
    if (window) glfwPollEvents();
}
