#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
using namespace std;
 
typedef struct {
    GLenum       type;
    const char*  filename;
    GLuint       shader;
} ShaderInfo;
 
GLuint LoadShaders(ShaderInfo*);