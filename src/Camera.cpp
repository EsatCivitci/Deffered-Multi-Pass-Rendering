
#include "../headers/Camera.h"


Camera::Camera (const glm::quat& qUp,
    const glm::quat& qDown)
    : q_up(qUp),
      q_down(qDown)  
{
    q_gaze = glm::slerp(q_up, q_down, t);
    updateCameraVectors();
}


void Camera::setCamPos(glm::vec3 pos) {
    position = pos;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCamPosition() const
{
    return position;
}

glm::vec3 Camera::getFrontVector() const 
{
    return front;
}

void Camera::updateCameraVectors()
{
    front = glm::normalize(q_gaze * glm::vec3(0.0f, 0.0f, -1.0f));
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}  

void Camera::startMovement(float xpos, float ypos)
{
    isMoving = true;
    startPos = glm::vec2(xpos, ypos);
}

void Camera::endMovement()
{
    isMoving = false;
}

void Camera::moveCam(float xPos, float yPos)
{
    if (!isMoving) return;

    glm::vec2 currPos = glm::vec2(xPos, yPos);

    float hChange = currPos.x - startPos.x;
    float vChange = currPos.y - startPos.y;

    startPos = currPos;

    // Horizontal Rotation
    float hAngle = -hChange * 0.1f;
    glm::quat hQuat = glm::angleAxis(glm::radians(hAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    q_up = glm::normalize(hQuat * q_up);
    q_down = glm::normalize(hQuat * q_down);

    // Vertical Rotation
    float deltaT = -vChange * 0.0025;
    t += deltaT;
    t = glm::clamp(t, 0.0f, 1.0f);

    q_gaze = glm::normalize(glm::slerp(q_up, q_down, t));
    updateCameraVectors();
}


