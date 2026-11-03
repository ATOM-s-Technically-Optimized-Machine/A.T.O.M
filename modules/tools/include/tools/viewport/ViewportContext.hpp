#ifndef VIEWPORT_CONTEXT_HPP_
#define VIEWPORT_CONTEXT_HPP_

#include <imgui.h>
#include <glm/glm.hpp>
#include "inputs/ECSIntegration.hpp"
#include "core/MathUtils.hpp"

struct ViewportContext {
    Atom::Input::CameraComponent &cam;
    ImVec2 min;
    ImVec2 size;
    glm::mat4 view_proj;

    ViewportContext(Atom::Input::CameraComponent &cam, ImVec2 min, ImVec2 size)
    : cam(cam), min(min), size(size), view_proj(cam.camera.viewProjectionMatrix())
    {}
};

inline ImVec2 toScreen(const glm::vec4 &clip, const ViewportContext &view)
{
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    float u = view.cam.viewport.x + (ndc.x * 0.5f + 0.5f) * view.cam.viewport.width;
    float v = view.cam.viewport.y + (ndc.y * 0.5f + 0.5f) * view.cam.viewport.height;
    return ImVec2(view.min.x + u * view.size.x, view.min.y + (1.0f - v) * view.size.y);
}

inline bool project(const ViewportContext &view, glm::vec3 world, ImVec2 &out)
{
    glm::vec4 clip = view.view_proj * glm::vec4(world, 1.0f);
    if (clip.w <= 0.0f)
        return false;
    out = toScreen(clip, view);
    return true;
}

inline Ray mouseRay(const ViewportContext &view)
{
    ImVec2 m = ImGui::GetMousePos();
    float u = (m.x - view.min.x) / view.size.x;
    float v = 1.0f - (m.y - view.min.y) / view.size.y;

    const auto &cam_vp = view.cam.viewport;
    float ndc_x = ((u - cam_vp.x) / cam_vp.width) * 2.0f - 1.0f;
    float ndc_y = ((v - cam_vp.y) / cam_vp.height) * 2.0f - 1.0f;

    glm::mat4 inv_view_proj = glm::inverse(view.view_proj);

    glm::vec4 near_pt = inv_view_proj * glm::vec4(ndc_x, ndc_y, -1.0f, 1.0f);
    glm::vec4 far_pt = inv_view_proj * glm::vec4(ndc_x, ndc_y, 1.0f, 1.0f);
    near_pt /= near_pt.w;
    far_pt /= far_pt.w;

    glm::vec3 origin = glm::vec3(near_pt);
    glm::vec3 dir = glm::normalize(glm::vec3(far_pt - near_pt));

    return Ray(origin, dir);
}

#endif /* VIEWPORT_CONTEXT_HPP_ */