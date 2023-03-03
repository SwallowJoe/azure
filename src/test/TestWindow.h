#pragma once

#include <iostream>
#include "Window.h"

#define  BUFFER_OFFSET(offset) ((void *)(offset))

class TestWindow : public Window {
public:
    void earlyInit() override;
    void init() override;
    void display(int64_t frameId) override;

    TestWindow(const char* title, int width, int height);
    ~TestWindow();

private:
    static const GLuint NumVertices = 6;

    enum Buffer_IDs { ArrayBuffer, NumBuffers };
    enum Attrib_IDs { vPosition = 0 };
    enum VAO_IDs { Triangles, NumVAOs };

    GLuint VAOs[NumVAOs];
    GLuint Buffers[NumBuffers];
};