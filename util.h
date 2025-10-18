#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;


static int winWidth = 800;
static int winHeight = 560;
static int VERSION = 4;
static int max_iteration = 100;
static double centerX = -0.75, centerY = 0.0;
static double scale = 1.5;
static double dragStartX = 0, dragStartY = 0;
static double dragCenterX = 0, dragCenterY = 0;
static bool dragging = false;
static double aspect = 1.0;

enum ShaderOrProgram {
    SHADER,
    PROGRAM
};

inline void check_error(const GLuint shader, const GLsizei status, const string &message,
                        const ShaderOrProgram mode = SHADER) {
    int success;
    char infoLog[512];
    switch (mode) {
        case SHADER:
            glGetShaderiv(shader, status, &success);
            break;
        case PROGRAM:
            glGetProgramiv(shader, status, &success);
            break;
    }
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        cerr << message << infoLog << "\n" << endl;
        exit(EXIT_FAILURE);
    }
}

inline GLFWwindow *window_preset() {
    // Initialize GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VERSION);

    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFWindow object of
    auto *window = glfwCreateWindow(winWidth, winHeight, "Homework1", nullptr, nullptr);

    // Error check if the window fails to create
    if (window == nullptr) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(1);
    }

    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    // Enable vsync
    glfwSwapInterval(1);

    // Load GLAD so it configures OpenGL
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        cerr << "Failed to initialize GLAD" << "\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    const GLubyte *version = glGetString(GL_VERSION);
    cout << "OpenGL version: " << version << "\n";

    return window;
}

inline string load_shader_source(const char *filePath) {
    const ifstream file(filePath, ios::in | ios::binary);
    if (!file) {
        cerr << "Failed to open shader file: " << filePath << endl;
        exit(EXIT_FAILURE);
    }
    ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}


inline GLuint load_compile_shader(const char *filePath, const GLenum shader_type) {
    const auto source = load_shader_source(filePath);

    // Compile shader
    const char *sourcePtr = source.c_str();
    const auto shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);
    return shader;
}


#endif
