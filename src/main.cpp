#include <iostream>
#include <signal.h>
#include <unistd.h>

#include "Window.h"
#include "Choreographer.h"

#include "test/TestWindow.h"

#include "Looper.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

void sig(int signum) {
	std::cout<<"我收到了Ctrl+c信号"<<std::endl;

    exit(0);
}

int main(int argc, char** argv){
    Window* win = new TestWindow("OpenGL", SCR_WIDTH, SCR_HEIGHT);
    win->show();

    signal(SIGINT,sig);
    pause();
    return 0;
}