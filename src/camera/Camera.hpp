#pragma once

#include <glm/glm.hpp>

enum class CameraDirection { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
public:
    Camera(glm::vec3 position, float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void processKeyboard(CameraDirection direction, float deltaTime);
    void processMouse(float xOffset, float yOffset);

    glm::vec3 getPosition() const { return m_position; }

private:
    void updateVectors();

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;

    float m_yaw;
    float m_pitch;
    float m_movementSpeed    = 2.5f;
    float m_mouseSensitivity = 0.1f;
    float m_fov              = 45.0f;
};
