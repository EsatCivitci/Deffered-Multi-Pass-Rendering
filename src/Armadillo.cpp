#include "../headers/Armadillo.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Armadillo::Armadillo(const std::string& objPath)
                    : VAO(0), VBO(0), EBO(0), objFile(objPath) {
    load();
    initVBO();
}

Armadillo::~Armadillo() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Armadillo::load() {
    loadOBJ();
}


void Armadillo::loadOBJ() {
    std::ifstream file(objFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << objFile << std::endl;
        return;
    }

    std::vector<float> tempVertices;
    std::vector<float> tempNormals;
    std::vector<float> tempTexCoords;
    std::vector<unsigned int> tempIndices;
    std::vector<unsigned int> tempNormalIndices;
    
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;
        
        if (prefix == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            tempVertices.push_back(x);
            tempVertices.push_back(y);
            tempVertices.push_back(z);
        } 
        
        else if (prefix == "vn") {
            float nx, ny, nz;
            ss >> nx >> ny >> nz;
            tempNormals.push_back(nx);
            tempNormals.push_back(ny);
            tempNormals.push_back(nz);
        } 
        
        else if (prefix == "vt") {
            float u, v;
            ss >> u >> v;
            tempTexCoords.push_back(u);
            tempTexCoords.push_back(v);
        } 
        
        else if (prefix == "f") {
            unsigned int vIdx[3], nIdx[3];
            char slash;
            for (int i = 0; i < 3; i++) {
                ss >> vIdx[i] >> slash >> slash >> nIdx[i];
                tempIndices.push_back(vIdx[i] - 1);
                tempNormalIndices.push_back(nIdx[i] - 1);
            }
        }
    }

    vertices = tempVertices;
    normals = tempNormals;
    indices = tempIndices;


    // for (int i = 0; i < vertices.size(); i+=3) {
    //     std::cout << vertices[i] << "--" << vertices[i+1] << "--" << vertices[i+2] << std::endl;
    // }

    file.close();
}

void Armadillo::initVBO() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() + normals.size()) * sizeof(float), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), normals.size() * sizeof(float), normals.data()); 

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertices.size() * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //std::cout << VBO << std::endl;
}


void Armadillo::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
