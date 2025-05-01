#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;


class Qubit {
private:
    complex<double> alpha; 
    complex<double> beta;  
    
public:
    Qubit() : alpha(1.0, 0.0), beta(0.0, 0.0) {}
    
    void applyHadamard() {
        complex<double> new_alpha = (alpha + beta) / sqrt(2.0);
        complex<double> new_beta = (alpha - beta) / sqrt(2.0);
        alpha = new_alpha;
        beta = new_beta;
    }
    
    void applyPauliX() {
        swap(alpha, beta);
    }
    
    void applyPauliY() {
        complex<double> i(0.0, 1.0);
        complex<double> new_alpha = -i * beta;
        complex<double> new_beta = i * alpha;
        alpha = new_alpha;
        beta = new_beta;
    }
    
    void applyPauliZ() {
        beta = -beta;
    }
    
    void rotateX(double theta) {
        complex<double> c(cos(theta/2), 0.0);
        complex<double> s(0.0, -sin(theta/2));
        complex<double> new_alpha = c * alpha + s * beta;
        complex<double> new_beta = s * alpha + c * beta;
        alpha = new_alpha;
        beta = new_beta;
    }
    
    vec3 getBlochVector() const {
        double x = 2.0 * (alpha.real() * beta.real() + alpha.imag() * beta.imag());
        double y = 2.0 * (alpha.imag() * beta.real() - alpha.real() * beta.imag());
        double z = alpha.real() * alpha.real() + alpha.imag() * alpha.imag() - 
                   beta.real() * beta.real() - beta.imag() * beta.imag();
        return vec3(x, y, z);
    }
};


GLuint sphereVAO, sphereVBO, sphereEBO;
GLuint shaderProgram;
Qubit qubit;
float rotationAngle = 0.0f;
bool animate = false;


GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cerr << "Shader compilation error:\n" << infoLog << endl;
    }
    return shader;
}


void createShaderProgram() {
    const char* vertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )glsl";

    const char* fragmentShaderSource = R"glsl(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 color;
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )glsl";

    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "Shader program linking error:\n" << infoLog << endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void createSphere(float radius, int sectors, int stacks) {
    vector<vec3> vertices;
    vector<unsigned int> indices;
    
    float x, y, z, xy;
    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;
    
    for (int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cos(stackAngle);
        z = radius * sin(stackAngle);
        
        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;
            x = xy * cos(sectorAngle);
            y = xy * sin(sectorAngle);
            vertices.push_back(vec3(x, y, z));
        }
    }
    
    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;
        
        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    
    glBindVertexArray(sphereVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void drawSphere(mat4 model, vec3 color) {
    glUseProgram(shaderProgram);
    
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
    glUniform3f(colorLoc, color.x, color.y, color.z);
    
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, 960, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void drawVector(vec3 start, vec3 end, vec3 color) {
    glUseProgram(shaderProgram);
    
    
    vec3 direction = end - start;
    float length = length(direction);
    direction = normalize(direction);
    
    vec3 axis = cross(vec3(0.0f, 1.0f, 0.0f), direction);
    float angle = acos(dot(vec3(0.0f, 1.0f, 0.0f), direction));
    
    mat4 model = translate(mat4(1.0f), start);
    model = rotate(model, angle, axis);
    model = scale(model, vec3(0.05f, length, 0.05f));
    
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
    glUniform3f(colorLoc, color.x, color.y, color.z);
    
    
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, 960, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_H:
                qubit.applyHadamard();
                break;
            case GLFW_KEY_X:
                qubit.applyPauliX();
                break;
            case GLFW_KEY_Y:
                qubit.applyPauliY();
                break;
            case GLFW_KEY_Z:
                qubit.applyPauliZ();
                break;
            case GLFW_KEY_R:
                qubit.rotateX(0.1);
                break;
            case GLFW_KEY_SPACE:
                animate = !animate;
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
        }
    }
}

int main() {
    
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "Quantum Bloch Sphere Visualizer", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }
    
    
    glEnable(GL_DEPTH_TEST);
    
    
    createShaderProgram();
    createSphere(1.0f, 20, 20);
    
    
    mat4 projection = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUseProgram(shaderProgram);
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));
    
    
    mat4 view = lookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
    
    
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (animate) {
            qubit.rotateX(0.01f);
            rotationAngle += 0.01f;
        }
        
        
        mat4 sphereModel = mat4(1.0f);
        drawSphere(sphereModel, vec3(0.2f, 0.2f, 0.5f));
        
        
        vec3 blochVector = qubit.getBlochVector();
        drawVector(vec3(0.0f), blochVector, vec3(1.0f, 0.0f, 0.0f));
        
        
        drawVector(vec3(0.0f), vec3(1.5f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
        drawVector(vec3(0.0f), vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        drawVector(vec3(0.0f), vec3(0.0f, 0.0f, 1.5f), vec3(0.0f, 0.0f, 1.0f));
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();
    return 0;
}
