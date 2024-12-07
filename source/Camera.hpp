#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const float CAMERA_FOV_FACTOR = 1.0f;

enum class Direction : char
{
    FORWARD  = 0,
    BACKWARD = 1,
    LEFT     = 2,
    RIGHT    = 3,
    UPWARD   = 4,
    DOWNWARD = 5,
};

struct Camera
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front    = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right    = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw   = -90.0f;
    float pitch = 0.0f;

    float speed       = 0.5f;
    float sensitivity = 0.1f;
    float fov         = 45.0f;

    void update()
    {
        glm::vec3 temp_front;
        temp_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        temp_front.y = sin(glm::radians(pitch));
        temp_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        front = glm::normalize(temp_front);
        right = glm::normalize(glm::cross(front, world_up));
        up    = glm::normalize(glm::cross(right, front));
    }

    void FOV(int value)
    {
        if(value > 0)
            fov -= CAMERA_FOV_FACTOR;
        else
            fov += CAMERA_FOV_FACTOR;
    }

    void navigate(int x, int y)
    {
        yaw += x * sensitivity;
        pitch += y * sensitivity;

        yaw   = fmod(yaw, 360.0f);
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        update();
    }

    void move(Direction direction)
    {
        // clang-format off
        switch(direction)
        {
            case Direction::FORWARD:    position += front * speed; break;
            case Direction::BACKWARD:   position -= front * speed; break;
            case Direction::RIGHT:      position += right * speed; break;
            case Direction::LEFT:       position -= right * speed; break;
            case Direction::UPWARD:     position += up * speed; break;
            case Direction::DOWNWARD:   position -= up * speed; break;
        }
        // clang-format on
    }

    glm::mat4 model()
    {
        return glm::scale(
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
            glm::vec3(1.0f, 1.0f, 1.0f));
    }

    glm::mat4 view()
    {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 projection(int width, int height)
    {
        return glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
    }
};
