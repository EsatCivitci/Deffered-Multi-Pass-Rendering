#include "../headers/Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    string vertexCode;
    string fragmentCode;

    ifstream vShaderFile(vertexPath);
    ifstream fShaderFile(fragmentPath);

    stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    if (!vShaderFile.is_open()) {
        std::cerr << "Failed to open vertex shader file: " << vertexPath << std::endl;
    }

    fShaderStream << fShaderFile.rdbuf();
    if (!fShaderFile.is_open()) {
        std::cerr << "Failed to open fragment shader file: " << fragmentPath << std::endl;
    }

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    const char* vShaderSource = vertexCode.c_str();
    const char* fShaderSource = fragmentCode.c_str();

    GLuint vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderSource, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderSource, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

void Shader::checkCompileErrors(GLuint shader, const string &type) const {
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM"){
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success){
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            cerr << "Shader compilation error of type: " << type << "\n"
                 << infoLog << endl;
        }
    }else{
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success){
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            cerr << "Program linking error of type: " << type << "\n"
                 << infoLog << endl;
        }
    }
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setVec2(const std::string &name, const glm::vec2 &vec) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));   
}

void Shader::setFloat(const std::string &name, const float val) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::setInt(const std::string &name, const int val) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::setBool(const std::string &name, const bool val) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), val);
}

