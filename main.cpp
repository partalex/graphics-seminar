#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "util.h"

// Square parameters
constexpr float squareVertices[] = {
    -1, -1,
    1, -1,
    1, 1,
    -1, 1,
};

// Two triangles for the square
const unsigned int squareIndices[] = {
    0, 1, 2, // first triangle
    2, 3, 0 // second triangle
};

// Indices for the border (line strip)
const unsigned int borderIndices[] = {
    // for GL_LINE_STRIP
    0, 1, 2, 3, 0
};

int main() {
    const auto window = window_preset();

    // load vertex and fragment shader source code
    const auto vertex_shader_source = load_shader_source("../shaders/vertex.glsl");
    const auto fragment_shader_source = load_shader_source("../shaders/fragment.glsl");

    // Create and compile vertex shader
    const auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const auto *vertexSourcePtr = vertex_shader_source.c_str();
    glShaderSource(vertexShader, 1, &vertexSourcePtr, nullptr);
    glCompileShader(vertexShader);

    // Check shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create and compile fragment shader
    const auto *fragmentSourcePtr = fragment_shader_source.c_str();
    const auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSourcePtr, nullptr);
    glCompileShader(fragmentShader);

    // Check shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders into a program
    const auto shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    // Check program linking
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete shaders as they're linked now
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Enable blending for the flame transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint VAO, VBO, EBO, borderEBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &borderEBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);

    // this bind is for the vertex shader's layout(location = 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    // Border EBO (for line strip)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, borderEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(borderIndices), borderIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Main while loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Update time for animation
        auto currentTime = static_cast<float>(glfwGetTime());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background for better flame effect
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        // Pass uniforms
        const auto loc_center = glGetUniformLocation(shader, "center");
        glUniform2f(loc_center, static_cast<float>(centerX), static_cast<float>(centerY));
        const auto loc_scale = glGetUniformLocation(shader, "scale");
        glUniform1f(loc_scale, static_cast<float>(scale));

        // Pass time uniform for animation
        const auto loc_time = glGetUniformLocation(shader, "time");
        glUniform1f(loc_time, currentTime);

        glBindVertexArray(VAO);

        // Draw filled square with flame effect
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glUniform1i(glGetUniformLocation(shader, "isBorder"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &borderEBO);
    glDeleteProgram(shader);

    // Delete window before ending the program
    glfwDestroyWindow(window);

    // Terminate GLFW before ending the program
    glfwTerminate();
    return 0;
}
