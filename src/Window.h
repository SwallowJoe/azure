#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Time.h"
#include "Choreographer.h"
#include "Log.h"

class Window {
public:
    Window(const char* title, int width, int height)
            : mThreadName("Ui"),
              mTitle(title),
              mWidth(width),
              mHeight(height) {
        mUiThread = std::thread([this]() {
            std::unique_lock<std::mutex> lock(mMutex);
            threadMain(lock);
        });
        pthread_setname_np(mUiThread.native_handle(), mThreadName);
    }

    ~Window() {
        mUiThread.join();
    }

    virtual void earlyInit() {

    }

    virtual void init() {

    }

    virtual void display(int64_t frameId) {

    }

    int show() {
        return 0;
    }

private:
    Window() = delete;

    void threadMain(std::unique_lock<std::mutex>& lock) {
        initEnv();

        while (!glfwWindowShouldClose(window))
        {
            Choreographer::getInstance()->postFrameCallback(frameCallback);
            mCondition.wait(lock);
        
            display(frameId++);
            glfwSwapBuffers(window);
            glfwPollEvents();
            // default for 60Hz.
            //mCondition.wait_for(lock,
            //        std::chrono::duration<nsecs_t, std::nano>(16666666));
        }

        glfwDestroyWindow(window);
        glfwTerminate();

        Choreographer::getInstance()->release();
    }

    int initEnv() {
        earlyInit();
        glfwInit();
        window = glfwCreateWindow(mWidth, mHeight, mTitle, nullptr, nullptr);
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
        return 0;
    }

    mutable std::mutex mMutex;
    mutable std::condition_variable mCondition;
    std::thread mUiThread;
    const char* const mThreadName;

    const char* const mTitle;
    int mWidth = 0;
    int mHeight = 0;

    int64_t frameId = 0;

    GLFWwindow* window;

    const std::function<void(int, nsecs_t)> frameCallback = [&](int frameId, nsecs_t timestamp) {
        // LOGI("onFrame");
        std::lock_guard lock(mMutex);
        mCondition.notify_all();
    };
};