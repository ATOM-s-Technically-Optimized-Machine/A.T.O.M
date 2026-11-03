#ifndef MATHUTILS_HPP_
    #define MATHUTILS_HPP_

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

// A point is affected by translation (w = 1), a direction isn't (w = 0)
inline glm::vec3 transformPoint(const glm::mat4 &m, glm::vec3 p)
{
    return glm::vec3(m * glm::vec4(p, 1.0f));
}

inline glm::vec3 transformDirection(const glm::mat4 &m, glm::vec3 d)
{
    return glm::vec3(m * glm::vec4(d, 0.0f));
}

inline bool rayPlane(const Ray &ray, glm::vec3 point, glm::vec3 n, glm::vec3 &hit)
{
    float denom = glm::dot(ray.direction, n);
    if (std::abs(denom) < 1e-4f)
        return false;
    float t = glm::dot(point - ray.origin, n) / denom;
    if (t < 0.0f)
        return false;
    hit = ray.origin + ray.direction * t;
    return true;
}

inline bool rayAABB(Ray ray, glm::vec3 bmin, glm::vec3 bmax, float &t_hit)
{
    float t_near = -FLT_MAX;
    float t_far = FLT_MAX;

    for (int a = 0; a < 3; a++) {
        if (std::abs(ray.direction[a]) < 1e-8f) {
            if (ray.origin[a] < bmin[a] || ray.origin[a] > bmax[a])
                return false;
            continue;
        }

        float t1 = (bmin[a] - ray.origin[a]) / ray.direction[a];
        float t2 = (bmax[a] - ray.origin[a]) / ray.direction[a];
        if (t1 > t2)
            std::swap(t1, t2);

        t_near = std::max(t_near, t1);
        t_far = std::min(t_far, t2);

        if (t_near > t_far)
            return false;
    }

    if (t_far < 0.0f) return false;
    t_hit = std::max(t_near, 0.0f);
    return true;
}

inline bool rayLineParam(const Ray &ray, glm::vec3 point, glm::vec3 dir, float &t)
{
    glm::vec3 w0 = point - ray.origin;
    float b = glm::dot(dir, ray.direction);
    float d = glm::dot(dir, w0);
    float e = glm::dot(ray.direction, w0);
    float denom = 1.0f - b * b;

    if (denom < 1e-4f)
        return false;
    t = (b * e - d) / denom;
    return true;
}

#endif /* !MATHUTILS_HPP_ */
