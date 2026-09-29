#ifndef ARMADILLO_H
#define ARMADILLO_H

#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>

class Armadillo {
public:
    Armadillo(const std::string& objPath);
    ~Armadillo();

    void load();

    void initVBO();
    void draw();

    void loadOBJ();

private:
    GLuint VAO, VBO, EBO;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned int> indices;

    std::string objFile;
};

#endif