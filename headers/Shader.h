#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace std;

class Shader{
public:
    GLuint ID;
    
    Shader(const char* vertexPath, const char* fragmentPath);

    void use() const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;
    void setVec2(const std::string &name, const glm::vec2 &vec) const;
    void setFloat(const std::string &name, const float val) const;
    void setInt(const std::string &name, const int val) const;
    void setBool(const std::string &name, const bool val) const;

private:
    void checkCompileErrors(GLuint shader, const string &type) const;
};

#endif 