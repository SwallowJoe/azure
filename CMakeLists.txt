cmake_minimum_required(VERSION 3.12)
project(azure VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# add_compile_options(-Wall -Wextra -pedantic -Werror)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++17")

# 通过设置变量CMAKE_BUILD_TYPE来配置编译类型，可设置为：Debug、Release、RelWithDebInfo、MinSizeRel等
set(CMAKE_BUILD_TYPE Debug)

#找OpenGL库
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

message ("OPENGL_INCLUDE_DIR=" ${OPENGL_INCLUDE_DIR})
message ("GLFW_INCLUDE_DIR=" ${GLFW_INCLUDE_DIR})

include_directories(src)

include_directories(src/base)
add_subdirectory(src/base)

include_directories(src/frame)
add_subdirectory(src/frame)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/test/TestWindow.cpp
        src/test/LoadShaders.cpp
)

target_link_libraries(${PROJECT_NAME} base frame -lGL -lGLEW -lglfw -lpthread)
