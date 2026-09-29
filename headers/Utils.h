#ifndef UTILS_H
#define UTILS_H

#include "Camera.h"
#include "Shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

void printVec3(const std::string& name, const glm::vec3& vec);

GLuint loadCubeMapTexture();


float calculateDeltaTime();

#endif