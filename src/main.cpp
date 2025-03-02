#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <stb_image.h>

// Shader loader
std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(){
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(800, 800, "Wireframe Cube", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, 800, 800);

// Vertices
float vertices[] = {
    //     COORDINATES      //     COLORS          //     TEX CORDS
    -0.5f, 0.0f,  0.5f,     1.0f,  1.0f,  1.0f,     0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,     1.0f,  1.0f,  1.0f,     0.0f, 1.0f,
     0.5f, 0.0f, -0.5f,     1.0f,  1.0f,  1.0f,     1.0f, 1.0f,
     0.5f, 0.0f,  0.5f,     1.0f,  1.0f,  1.0f,     1.0f, 0.0f,

    -0.5f, 1.0f,  0.5f,     1.0f,  1.0f,  1.0f,    0.0f, 0.0f,
    -0.5f, 1.0f, -0.5f,     1.0f,  1.0f,  1.0f,    0.0f, 1.0f,
     0.5f, 1.0f, -0.5f,     1.0f,  1.0f,  1.0f,    1.0f, 1.0f,
     0.5f, 1.0f,  0.5f,     1.0f,  1.0f,  1.0f,    1.0f, 0.0f
};

    
int indices[] =
{
    0, 1, 2,
    0, 2, 3,
    4, 5, 6,
    4, 6, 7,
    0, 3, 4,
    3, 4, 7,
    1, 2, 5,
    2, 5, 6,
    0, 1, 4,
    1, 4, 5,
    2, 3, 6,
    3, 6, 7
};

    // VAO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Shaders
    std::string vertexsource = loadShaderSource("Shaders/default.vert");
    const char* vertexShaderSource = vertexsource.c_str();
    std::string fragmentsource = loadShaderSource("Shaders/default.frag");
    const char* fragmentShaderSource = fragmentsource.c_str();

    // Vertex shader
    unsigned int vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, 0);
    glCompileShader(vertexShader);

    // Fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, 0);
    glCompileShader(fragmentShader);

    // Shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

   // EBO
   unsigned int EBO;
   glGenBuffers(1, &EBO);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   float rotation  = 0.0f;
   double prevTime = glfwGetTime();

   glEnable(GL_DEPTH_TEST);
   

   // While running
    while(!glfwWindowShouldClose(window)){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, -0.5f, -4.0f));
        proj = glm::perspective(glm::radians(45.0f), (float)(800 / 800), 0.1f, 100.0f);

        double crntTime = glfwGetTime();
        if(crntTime - prevTime >= 1 / 60){
            rotation += 0.59f;
            prevTime = crntTime;
        }
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

        glUseProgram(shaderProgram);

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}