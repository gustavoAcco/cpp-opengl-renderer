#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

static constexpr glm::vec3 WORLD_UP = {0.0f, 1.0f, 0.0f};

Camera::Camera(glm::vec3 position, float yaw, float pitch)
    : m_position(position)
    , m_front(0.0f, 0.0f, -1.0f)
    , m_up(WORLD_UP)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_yaw(yaw)
    , m_pitch(pitch)
{
    updateVectors();
}

void Camera::updateVectors()
{
    glm::vec3 front;
    front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    front.y = std::sin(glm::radians(m_pitch));
    front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, WORLD_UP));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::processKeyboard(CameraDirection direction, float deltaTime)
{
    float velocity = m_movementSpeed * deltaTime;

    switch (direction) {
        case CameraDirection::FORWARD:  m_position += m_front * velocity; break;
        case CameraDirection::BACKWARD: m_position -= m_front * velocity; break;
        case CameraDirection::LEFT:     m_position -= m_right * velocity; break;
        case CameraDirection::RIGHT:    m_position += m_right * velocity; break;
    }
}

void Camera::processMouse(float xOffset, float yOffset)
{
    m_yaw   += xOffset * m_mouseSensitivity;
    m_pitch += yOffset * m_mouseSensitivity;

    // Clamp pitch to prevent gimbal flip
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    updateVectors();
}
