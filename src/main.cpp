#include <iostream>
#include "Window.h"
#include "test/TestWindow.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

int main(int argc, char** argv){
    Window* win = new TestWindow("OpenGL", SCR_WIDTH, SCR_HEIGHT);
    win->show();
    return 0;
}