#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> 
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera (const glm::quat& qUp,
            const glm::quat& qDown);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getCamPosition() const;
    glm::vec3 getFrontVector() const;

    void setCamPos(glm::vec3 pos);

    void startMovement(float xpos, float ypos);
    void endMovement();
    void moveCam(float xPos, float yPos);

private:
    glm::vec3 position = {0.0f,0.0f,0.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    glm::vec3 front = {0.0f, 0.0f, -1.0f};
    glm::vec3 right;

    glm::quat q_up;
    glm::quat q_down;
    glm::quat q_gaze;
    float t = 0.5f; 

    bool isMoving = false;
    glm::vec2 startPos;


    float movementSpeed;
    float mouseSensitivity;

    void updateCameraVectors();

};

#endif 

