#ifndef MATHUTILS_HPP_
    #define MATHUTILS_HPP_

#include <glm/glm.hpp>

// A point is affected by translation (w = 1), a direction isn't (w = 0)
inline glm::vec3 transformPoint(const glm::mat4 &m, glm::vec3 p)
{
    return glm::vec3(m * glm::vec4(p, 1.0f));
}

inline glm::vec3 transformDirection(const glm::mat4 &m, glm::vec3 d)
{
    return glm::vec3(m * glm::vec4(d, 0.0f));
}

#endif /* !MATHUTILS_HPP_ */
