#ifndef TRANSFORMTOOLS_HPP_
#define TRANSFORMTOOLS_HPP_

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

        void buildHandles(const ViewportContext &, const TransformComponent &, std::vector<Handle> &) const override;

        bool isLocal() const { return m_local; }
    private:
        bool m_local = false;
};

class RotateTool : public ITool {
    public:
        const char *name() const override { return "Rotate"; }
        ImGuiKey shortcut() const override { return ImGuiKey_R; }

        void buildHandles(const ViewportContext &, const TransformComponent &, std::vector<Handle> &) const override;
};

#endif /* TRANSFORMTOOLS_HPP_ */