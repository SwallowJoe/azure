#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(const char* title, int width, int height)
            : mTitle(title),
              mWidth(width),
              mHeight(height) {

    }

    ~Window() = default;

    virtual void earlyInit() {

    }

    virtual void init() {

    }

    virtual void display() {

    }

    int show() {
        earlyInit();
        glfwInit();
        GLFWwindow* window = glfwCreateWindow(mWidth, mHeight, mTitle, nullptr, nullptr);
        if (window == NULL)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);

        if(glewInit() != GLEW_OK){
            std::cerr << "Failed to initalize GLEW" << std::endl;
            return -1;
        }

        init();
        while (!glfwWindowShouldClose(window))
        {
            display();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
private:
    Window() = delete;

    const char* mTitle;
    int mWidth = 0;
    int mHeight = 0;
};