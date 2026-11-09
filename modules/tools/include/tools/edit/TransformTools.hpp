#ifndef TRANSFORMTOOLS_HPP_
#define TRANSFORMTOOLS_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "tools/edit/ITool.hpp"

class SelectTool : public ITool {
    public:
        const char *name() const override { return "Select"; }
        ImGuiKey shortcut() const override { return ImGuiKey_A; }
};

class MoveTool : public ITool {
    public:
        const char *name() const override { return "Move"; }
        ImGuiKey shortcut() const override { return ImGuiKey_M; }

        void options() override { ImGui::Checkbox("Local", &m_local); }
        void onKeys() override
        {
            if (ImGui::IsKeyChordPressed(ImGuiKey_L))
                m_local = !m_local;
        }

        void buildHandles(const ViewportContext &, ECS::ECS &, ECS::EntityID, std::vector<Handle> &) const override;

        bool beginDrag(const Handle &, const Ray &, ECS::ECS &, ECS::EntityID) override;
        bool updateDrag(const Ray &, ECS::ECS &, ECS::EntityID) override;
    private:
        bool m_local = false;
        glm::vec3 m_dir;
        glm::vec3 m_start_pos;
        float m_start_t;
};

class RotateTool : public ITool {
    public:
        const char *name() const override { return "Rotate"; }
        ImGuiKey shortcut() const override { return ImGuiKey_R; }

        void buildHandles(const ViewportContext &, ECS::ECS &, ECS::EntityID, std::vector<Handle> &) const override;

        bool beginDrag(const Handle &, const Ray &, ECS::ECS &, ECS::EntityID) override;
        bool updateDrag(const Ray &, ECS::ECS &, ECS::EntityID) override;
    private:
        glm::vec3 m_axis;
        glm::quat m_start_rot;
        glm::vec3 m_start_vec;
};

#endif /* TRANSFORMTOOLS_HPP_ */