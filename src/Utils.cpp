#include "../headers/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../headers/stb_image.h"

#include <iostream>
#include <vector>
#include <random>

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H


using namespace std;

extern Camera* camera;

//extern glm::mat4 model;
extern glm::mat4 view;
extern glm::mat4 projection;

extern int gWidth;          
extern int gHeight; 



float currTime = 0;
float totalTAmount = 0;


void printVec3(const std::string& name, const glm::vec3& vec) {
    std::cout << name << " = (" 
              << vec.x << ", " 
              << vec.y << ", " 
              << vec.z << ")\n";
}


GLuint loadCubeMapTexture() {
    GLuint gTexCube;
    glGenTextures(1, &gTexCube);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gTexCube);

    const char* images[] = {"images/px.hdr","images/nx.hdr",
                            "images/py.hdr","images/ny.hdr",
                            "images/pz.hdr","images/nz.hdr"};

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    stbi_set_flip_vertically_on_load(false);

    for (int i = 0; i < 6; ++i) {
        int width, height, nrChannels;
        float* data = stbi_loadf(images[i], &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Failed to load cubemap face: " << images[i] << std::endl;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
        stbi_image_free(data);


    }

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        

    return gTexCube;

}


