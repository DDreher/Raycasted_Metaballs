#pragma once

#include "glm/glm.hpp"

struct Particle
{
    glm::vec3 pos;
    glm::vec3 speed;
    float life = 0.0f; // Remaining life of the particle. if <= 0: unused / dead.
    float squared_cam_distance = 1.0f; // -1.0 if dead

    bool operator<(const Particle& that) const
    {
        return this->life < that.life;
    }
};