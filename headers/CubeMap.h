#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <vector>
#include <string>
#include <GL/glew.h>

class CubeMap {
public:
    CubeMap();
    ~CubeMap();

    void draw();
    void initVBO(); 
private:
    GLuint VAO, VBO, EBO;
            
    std::vector<float> vertices = {
        // +X
        1, -1, -1,  1, -1,  1,  1,  1,  1,
        1,  1,  1,  1,  1, -1,  1, -1, -1,
    
        // -X 
       -1, -1,  1, -1, -1, -1, -1,  1, -1,
       -1,  1, -1, -1,  1,  1, -1, -1,  1,
    
        // +Y 
       -1,  1,  1,  1,  1,  1,  1,  1, -1,
        1,  1, -1, -1,  1, -1, -1,  1,  1,
    
        // -Y 
       -1, -1, -1,  1, -1, -1,  1, -1,  1,
        1, -1,  1, -1, -1,  1, -1, -1, -1,
    
        // +Z 
       -1, -1,  1,  1, -1,  1,  1,  1,  1,
        1,  1,  1, -1,  1,  1, -1, -1,  1,
    
        // -Z
        1, -1, -1, -1, -1, -1, -1,  1, -1,
       -1,  1, -1,  1,  1, -1,  1, -1, -1
    };
};

#endif