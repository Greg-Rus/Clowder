#pragma once
#include <glm/glm.hpp>

struct OrientationComponent
{
    glm::vec2 orientation;

    OrientationComponent(glm::vec2 orientation = glm::vec2(0.0, 0.0))
    {
        this->orientation = orientation;
    }
};