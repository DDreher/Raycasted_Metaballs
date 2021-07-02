#pragma once

#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Controller
{
public:
    Controller();
    virtual ~Controller();

    void Update(float delta);
    void Init(GLFWwindow* w);

    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetPosition() const;

private:
    glm::vec3 position = glm::vec3(30.0f, 10.0f, 12.0f);
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;

    float horizontal_angle = 4.0f;
    float vertical_angle = 6.0f;
    float initial_fov = 45.0f;
    float speed = 30.0f;
    float mouse_speed = 0.25f;

    GLFWwindow* window = nullptr;

    glm::mat4 mat_projection;
    glm::mat4 mat_view;

};

