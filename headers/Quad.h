#ifndef QUAD_H
#define QUAD_H

#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>

class Quad {
public:
    Quad();
    ~Quad();

    void initVBO();
    void draw();

private:
    unsigned int VAO, VBO;


    float vertices[24] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

};


#endif