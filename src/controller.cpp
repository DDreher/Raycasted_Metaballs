#include "controller.h"

Controller::Controller()
{
}

Controller::~Controller()
{
}

void Controller::Init(GLFWwindow* w)
{
    window = w;
}

void Controller::Update(float delta)
{
    // get mouse position
    double pos_x, pos_y;
    glfwGetCursorPos(window, &pos_x, &pos_y);

    int screen_width;
    int screen_height;
    glfwGetWindowSize(window, &screen_width, &screen_height);
    glfwSetCursorPos(window, screen_width * .5f, screen_height * .5f);

    // compute new orientation
    horizontal_angle += mouse_speed * delta * float(screen_width * .5f - pos_x);
    vertical_angle += mouse_speed * delta * float(screen_height * .5f - pos_y);

    // Spherical coordinates to Cartesian coordinates
    direction = glm::vec3
    (
        cos(vertical_angle) * sin(horizontal_angle),
        sin(vertical_angle),
        cos(vertical_angle) * cos(horizontal_angle)
    );

    right = glm::vec3
    (
        sin(horizontal_angle - 3.14f / 2.0f),
        0.0f,
        cos(horizontal_angle - 3.14f / 2.0f)
    );

    up = glm::cross(right, direction);

    // handle keyboard input
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += direction * delta * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position -= direction * delta * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += right * delta * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position -= right * delta * speed;
    }

    // Move up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        position += up * delta * speed;
    }

    // Move down
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        position -= up * delta * speed;
    }

    static const float ASPECT_RATIO = 4.0f / 3.0f;
    static const float CLIP_NEAR = 0.1f;
    static const float CLIP_FAR = 100.0f;
    mat_projection = glm::perspective(initial_fov, ASPECT_RATIO, CLIP_NEAR, CLIP_FAR);

    // Camera matrix
    mat_view = glm::lookAt
    (
        position,             // Camera is here
        position + direction, // and looks here
        up                    // Head is up
    );
}

glm::mat4 Controller::GetProjectionMatrix() const
{
    return mat_projection;
}

glm::mat4 Controller::GetViewMatrix() const
{
    return mat_view;
}

glm::vec3 Controller::GetPosition() const
{
    return position;
}
