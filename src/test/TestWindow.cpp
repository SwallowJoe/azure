#include "TestWindow.h"
#include "Log.h"
#include "LoadShaders.h"

TestWindow::TestWindow(const char* title, int width, int height) : Window(title, width, height) {
}

void TestWindow::earlyInit() {
    std::cout<<"TestWindow earlyInit() call"<<std::endl;
}

void TestWindow::init() {
    std::cout<<"TestWindow init() call"<<std::endl;

    static const GLfloat vertices[NumVertices][2] = {
        {-0.90, -0.90 }, // Triangle 1
        { 0.85, -0.90 },
        {-0.90,  0.85 },
        { 0.90, -0.85 }, // Triangle 2
        { 0.90,  0.90 },
        {-0.85,  0.90 }
    };

    glCreateVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);

    glCreateBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);

    glNamedBufferStorage(Buffers[ArrayBuffer], sizeof(vertices), vertices, 0);

    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "res/triangles.vert" },
        { GL_FRAGMENT_SHADER, "res/triangles.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders(shaders);
    glUseProgram(program);

    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(vPosition);

}

void TestWindow::display(int64_t frameId) {
    LOGI("TestWindow display() call ", frameId);
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static const float red[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    static const float green[] = { 0.0f, 1.0f, 0.0f, 0.0f };
    static const float blue[] = { 0.0f, 0.0f, 1.0f, 0.0f };
#ifdef USE_BG_FLASH
    if (frameId % 4 == 0) {
        glClearBufferfv(GL_COLOR, 0, black);
    } else if (frameId % 4 == 1) {
        glClearBufferfv(GL_COLOR, 0, red);
    } else if (frameId % 4 == 2) {
        glClearBufferfv(GL_COLOR, 0, green);
    } else if (frameId % 4 == 3) {
        glClearBufferfv(GL_COLOR, 0, blue);
    } else {
        glClearBufferfv(GL_COLOR, 0, black);
    }
#else
    glClearBufferfv(GL_COLOR, 0, black);
#endif

    glBindVertexArray(VAOs[Triangles]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
 
    glFlush();
}