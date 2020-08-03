#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "controller.h"
#include "particle.h"
#include "shader.h"

GLFWwindow* window = nullptr;
Controller controller;

int screen_width = 1024;
int screen_height = 768;
bool is_stopped = false;	// if true sets delta to 0.0
bool is_slowmo = false;		// if true multiplies delta with .5

static constexpr int MAX_PARTICLES = 75;
Particle particles[MAX_PARTICLES];
glm::vec4 particle_positions_camera[MAX_PARTICLES]; // positions of all particles in camera space
int last_used_particle_id = 0;
float emitter_frequency = 0.05f;    // how long to wait before a particle is emitted?
float emitter_timer = 0.0f;		    // timer to check if we have to emit a particle

int FindFreeParticle()
{
    for (int i = last_used_particle_id; i < MAX_PARTICLES; ++i)
    {
        if (particles[i].life < 0)
        {
            last_used_particle_id = i;
            return i;
        }
    }

    for (int i = 0; i < last_used_particle_id; i++)
    {
        if (particles[i].life < 0)
        {
            last_used_particle_id = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void SortParticles()
{
    std::sort(&particles[0], &particles[MAX_PARTICLES]);
}


int main()
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screen_width, screen_height, "GPU-based Raycasted Meta Balls", nullptr, nullptr);
    if (window == nullptr)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    controller.Init(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPos(window, screen_width / 2, screen_height / 2);

    glClearColor(0.392f, 0.584f, 0.929f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);

    // Accept fragment if it's closer to the camera than other ones
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glAlphaFunc(GL_GREATER, 0.0f);

    GLuint vertex_array_handle;
    glGenVertexArrays(1, &vertex_array_handle);
    glBindVertexArray(vertex_array_handle);

    GLuint point_shader_handle = LoadShaders("assets/PointVertexShader.glsl", "assets/PointFragmentShader.glsl");
    GLuint billboard_shader_handle = LoadShaders("assets/BillboardVertexShader.glsl", "assets/BillboardGeometryShader.glsl", "assets/BillboardFragmentShader.glsl");
    GLuint fluid_shader_handle = LoadShaders("assets/FluidVertexShader.glsl", "assets/FluidGeometryShader.glsl", "assets/FluidFragmentShader.glsl");

    GLfloat particle_position_data[MAX_PARTICLES * 4];

    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        particles[i].life = -1.0f;
    }

    GLuint particle_pos_vbo_handle;
    glGenBuffers(1, &particle_pos_vbo_handle);
    glBindBuffer(GL_ARRAY_BUFFER, particle_pos_vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // Set up uniforms
    GLuint mat_mvp_handle = glGetUniformLocation(fluid_shader_handle, "MVP");
    GLuint mat_view_handle = glGetUniformLocation(fluid_shader_handle, "V");
    GLuint mat_model_handle = glGetUniformLocation(fluid_shader_handle, "M");
    GLuint mat_projection_handle = glGetUniformLocation(fluid_shader_handle, "P");
    GLuint light_pos_handle = glGetUniformLocation(fluid_shader_handle, "light_pos_world");
    GLuint particle_pos_handle = glGetUniformLocation(fluid_shader_handle, "particle_positions_camera");
    GLuint mat_inverse_projection_handle = glGetUniformLocation(fluid_shader_handle, "P_inv");
    GLuint viewport_handle = glGetUniformLocation(fluid_shader_handle, "viewport");
    glUseProgram(fluid_shader_handle);

    // set our light position
    glm::vec4 lightPos_world = glm::vec4(10.0f, -5.0f, 40.0f, 1.0f);
    glUniform3f(light_pos_handle, lightPos_world.x, lightPos_world.y, lightPos_world.z);

    // set viewport uniform
    GLfloat viewport[4];
    glGetFloatv(GL_VIEWPORT, viewport);

    double last_time = glfwGetTime();

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 view_projection;
    glm::mat4 model;
    glm::mat4 mvp;
    glm::mat4 inv_projection;

    do
    {
        // clear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // compute delta
        double current_time = glfwGetTime();
        float delta = static_cast<float>(current_time - last_time);
        last_time = current_time;

        if (is_stopped)
        {
            delta = 0.0;
        }

        if (is_slowmo)
        {
            delta *= .5f;
        }

        int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
        if (focused > 0)
        {
            controller.Update(delta);
        }

        projection = controller.GetProjectionMatrix();
        view = controller.GetViewMatrix();
        view_projection = projection * view;
        model = glm::mat4(1.0f);
        mvp = projection * view * model;
        inv_projection = glm::inverse(projection);
        glm::vec3 camera_pos = controller.GetPosition();

        // Update particle emitter
        emitter_timer += delta;
        if (emitter_timer >= emitter_frequency)
        {
            // emit a particle
            emitter_timer = 0.0f;
            int particle_id = FindFreeParticle();
            particles[particle_id].life = 5.0f;
            particles[particle_id].pos = glm::vec3(-5.0f, 5.0f, -10.0f);
            float spread = 1.00f;
            glm::vec3 maindir = glm::vec3(10.0f, 0.0f, 0.0f);
            glm::vec3 randomdir = glm::vec3(
                (rand() % 2000 - 1000.0f) / 1000.0f,
                (rand() % 2000 - 1000.0f) / 1000.0f,
                (rand() % 2000 - 1000.0f) / 1000.0f
            );
            particles[particle_id].speed = maindir + randomdir * spread;
        }

        // Simulate all particles
        int particle_count = 0;
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            Particle& p = particles[i];

            if (p.life > 0.0f)
            {
                // Decrease life
                p.life -= delta;

                if (p.life > 0.0f)
                {
                    p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * delta * 0.5f;
                    p.pos += p.speed * delta;

                    if (p.pos.y < -5.0f)
                    {
                        p.speed.y = 0.0f;
                        p.speed.x *= 0.85f;
                    }

                    // Update buffer that's sent to the GPU
                    particle_position_data[4 * particle_count + 0] = static_cast<GLfloat>(p.pos.x);
                    particle_position_data[4 * particle_count + 1] = static_cast<GLfloat>(p.pos.y);
                    particle_position_data[4 * particle_count + 2] = static_cast<GLfloat>(p.pos.z);
                    particle_position_data[4 * particle_count + 3] = static_cast<GLfloat>(1.0f);
                }
                else
                {
                    p.life = -1.0f;
                }

                particle_count++;
            }
        }

        SortParticles();

        for (int i = 0; i < MAX_PARTICLES; ++i)
        {
            particle_positions_camera[i] = view * model * glm::vec4(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z, 1.0f);
        }

        // Rendering

        // update uniforms
        glUniformMatrix4fv(mat_mvp_handle, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(mat_model_handle, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(mat_view_handle, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(mat_projection_handle, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(mat_inverse_projection_handle, 1, GL_FALSE, &inv_projection[0][0]);
        glUniform4fv(particle_pos_handle, MAX_PARTICLES, value_ptr(particle_positions_camera[0]));
        glUniform4fv(viewport_handle, 1, &viewport[0]);

        // Update buffers on GPU
        glBindBuffer(GL_ARRAY_BUFFER, particle_pos_vbo_handle);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, particle_count * sizeof(GLfloat) * 4, particle_position_data);

        // 1st attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, particle_pos_vbo_handle);
        glVertexAttribPointer(
            0,                                // attribute id
            4,                                // size : x + y + z
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glVertexAttribDivisor(0, 1); // positions : one per particle (its center) -> 1

        // Draw the particles!
        glDrawArraysInstanced(GL_POINTS, 0, 4, particle_count);

        glDisableVertexAttribArray(0);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            glUseProgram(point_shader_handle);
            mat_mvp_handle = glGetUniformLocation(point_shader_handle, "MVP");
            mat_view_handle = glGetUniformLocation(point_shader_handle, "V");
            mat_model_handle = glGetUniformLocation(point_shader_handle, "M");
            mat_projection_handle = glGetUniformLocation(point_shader_handle, "P");
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            glUseProgram(billboard_shader_handle);
            mat_mvp_handle = glGetUniformLocation(billboard_shader_handle, "MVP");
            mat_view_handle = glGetUniformLocation(billboard_shader_handle, "V");
            mat_model_handle = glGetUniformLocation(billboard_shader_handle, "M");
            mat_projection_handle = glGetUniformLocation(billboard_shader_handle, "P");
            mat_inverse_projection_handle = -1;
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            glUseProgram(fluid_shader_handle);
            mat_mvp_handle = glGetUniformLocation(fluid_shader_handle, "MVP");
            mat_view_handle = glGetUniformLocation(fluid_shader_handle, "V");
            mat_model_handle = glGetUniformLocation(fluid_shader_handle, "M");
            mat_projection_handle = glGetUniformLocation(fluid_shader_handle, "P");
            light_pos_handle = glGetUniformLocation(fluid_shader_handle, "light_pos_world");
            particle_pos_handle = glGetUniformLocation(fluid_shader_handle, "particle_positions_camera");
            mat_inverse_projection_handle = glGetUniformLocation(fluid_shader_handle, "P_inv");
            viewport_handle = glGetUniformLocation(fluid_shader_handle, "viewport");
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Controls
        // Y key - Stop simulation
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        {
            is_stopped = true;
        }

        // X key - Enable slow motion and run simulation
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            is_stopped = false;
            is_slowmo = true;
        }

        // C key - disable slow motion and run simulation
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            is_stopped = false;
            is_slowmo = false;
        }
    } // As long as ESC key was not pressed or the window still open
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    //delete[] particle_position_data;
    //particle_position_data = nullptr;

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particle_pos_vbo_handle);
    glDeleteProgram(billboard_shader_handle);
    glDeleteProgram(point_shader_handle);
    glDeleteProgram(fluid_shader_handle);
    glDeleteVertexArrays(1, &vertex_array_handle);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
