#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "DebugGL.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderSource {
    std::string vertex;
    std::string fragment;
};

// ParseShader takes a file path that contains 2 sets of code for both a vertex shader and 
// a fragment shader. The code is put into a string stream and convert to a string
static ShaderSource ParseShader(const std::string& path) {
    enum class ShaderType {
        NONE = -1, 
        VERTEX = 0, 
        FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;
    std::ifstream stream(path);
    std::stringstream ss[2];
    std::string line;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else if(type != ShaderType::NONE) {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[(int)ShaderType::VERTEX].str(), ss[(int)ShaderType::FRAGMENT].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    const char* src = source.c_str();
    GLDebug(unsigned int id = glCreateShader(type));

    GLDebug(glShaderSource(id, 1, &src, nullptr));
    GLDebug(glCompileShader(id));

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        // Alloca can allocate memory outside of the stack if too big, leading to stack overflow exceptions. When used in
        // small usecases (like here) it is fine though. malloc (on the heap) is preferred when dealing with bigger
        // chunks of memory.
        char* message = (char*)alloca(length * sizeof(char)); 
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Error compiling shader [" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "]" << std::endl;
        std::cout << message << std::endl;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLDebug(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLDebug(glAttachShader(program, vs));
    GLDebug(glAttachShader(program, fs));
    GLDebug(glLinkProgram(program));
    GLDebug(glValidateProgram(program));

    GLDebug(glDeleteShader(vs));
    GLDebug(glDeleteShader(fs));

    return program;
}

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        std::cout << "There was a problem initializing glfw." << std::endl;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cout << "There was a problem creating a window." << std::endl;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "There was a problem initializing glew." << std::endl;
    }

    // New scope so that the vertex and index buffer get cleaned up before glfwTerminate(); is
    // called. OpenGL throws an error if it loses its context (even though there is no way to
    // catch that error, because there is not context). This means it can get stuck in an infinite
    // loop. In an engine you would want to solve that problem in a different way (smart pointers?).
    {
        // Vertexes for the buffer to use. Vertexes != position on screen.
        float positions[] = {
            -0.5f, -0.5f, // 0
             0.5f, -0.5f, // 1
             0.5f,  0.5f, // 2
            -0.5f,  0.5f  // 3
        };

        // Indices indicate which vertexes need to be drawn when. OpenGL can only draw triangles
        // by default, this means that for a square, you need to draw 2 triangles. It is very
        // inefficient to allocate memory for index 0 and 2 twice (these get used twice), so this
        // is where the indeces come in. 
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        unsigned int vArrayObject;
        GLDebug(glGenVertexArrays(1, &vArrayObject));
        GLDebug(glBindVertexArray(vArrayObject));

        VertexBuffer vb(positions, 8 * sizeof(float));

        GLDebug(glEnableVertexAttribArray(0));
        GLDebug(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

        IndexBuffer ib(indices, 6);

        // Shaders are the programs that run on the gpu. Basically anything done in c++ runs on the cpu, 
        // this is why we need to use this special method. Otherwise everything would run on the cpu
        // instead of the gpu, where you would want the graphics processing to be done.
        ShaderSource src = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(src.vertex, src.fragment);
        GLDebug(glUseProgram(shader));

        // Uniforms are used to get data into a shader. The shaders are code that run on the gpu and to
        // get data in there, you use uniforms. It works sort of like a parameter. In this case for the 
        // color of what we are drawing. 
        GLDebug(int location = glGetUniformLocation(shader, "u_Color"));
        GLAssert(location == -1);

        // Unbinding everything to simulate what you would need to do when you have more than 1 thing on
        // the screen. Because different things might need different buffers and programs. This way you
        // would have to set everything again, because you will never know if stuff is setup correctly.
        GLDebug(glBindVertexArray(0));
        GLDebug(glUseProgram(0));
        GLDebug(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLDebug(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLDebug(glClear(GL_COLOR_BUFFER_BIT));

            GLDebug(glUseProgram(shader));
            GLDebug(glUniform4f(location, 1.0f, 0.5f, 0.8f, 1.0f));

            GLDebug(glBindVertexArray(vArrayObject));
            ib.Bind();

            // Nullptr, because the index buffer is bound.
            GLDebug(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            /* Swap front and back buffers */
            GLDebug(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLDebug(glfwPollEvents());
        }

        GLDebug(glDeleteProgram(shader));
    }

    glfwTerminate();

    return 0;
}
