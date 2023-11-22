#include "DebugGL.h"

#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogError(const char* function, const char* file, int line) {
    bool result = false;

    while (GLenum error = glGetError() != GL_NO_ERROR) {
        std::cout << "OpenGL error [" << error << "] caused by: " << function << " on line " << line << " of [" << file << "]";
        result = true;
    }

    return result;
}
