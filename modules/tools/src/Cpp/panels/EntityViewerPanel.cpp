/*
** EPITECH PROJECT, 2026
** main
** File description:
** EntityViewerPanel
*/

#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include "tools/panels/EntityViewerPanel.hpp"
#include "core/ECS/Includes.hpp"
#include "imgui.h"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"
#include <cstdint>
#include <unordered_map>

EntityViewerPanel::EntityViewerPanel(editor::Editor &editor, ECS::ECS &ecs)
: APanel(editor), m_ecs(ecs)
{
}

EntityViewerPanel::~EntityViewerPanel()
{
}

void EntityViewerPanel::render(int view_id)
{
    if (m_editor.getSelectedEntity() == ECS::NULL_ENTITY) {
        ImGui::Text("No selected entity");
        return;
    }
    showEntityComponents();
}

void EntityViewerPanel::showEntityComponents()
{
    ECS::EntityID selected_entity = m_editor.getSelectedEntity();
    if (selected_entity== ECS::NULL_ENTITY)
        return;

    std::unordered_map<uint16_t, void *> components;
    m_ecs.entityGetAllComponents(components, selected_entity);
    std::size_t displayed_components = 0;

    ImGui::PushID((int)selected_entity);
    for (auto &[id, comp] : components) {
        if (m_inspector.registry().contains(id)) {
            ComponentDescriptor descriptor;
            m_inspector.registry()[id](descriptor, comp);
            displayed_components++;
            if (!ImGui::TreeNodeEx(descriptor.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                continue;
            for (int i = 0; i < descriptor.fields.size(); i++) {
                auto &entry = descriptor.fields[i];
                displayFieldEntry(entry, i);
            }
            ImGui::TreePop();
        }
    }
    ImGui::PopID();
    if (displayed_components == 0)
        ImGui::Text("%s", "Entity either containes no Component or they are not supported in the viewer");
}

void EntityViewerPanel::displayFieldEntry(FieldDescriptor entry, int id)
{
    ImGui::Text("%s", entry.name);
    ImGui::SameLine();
    std::string label = "##" + std::to_string(id);
    switch (entry.type) {
        case FieldType::Float:
            ImGui::InputFloat(label.c_str(), (float*)entry.ptr);
            break;
        case FieldType::Int:
            ImGui::InputInt(label.c_str(), (int*)entry.ptr);
            break;
        case FieldType::Vec3:
            ImGui::InputFloat3(label.c_str(), (float*)&((glm::vec3*)(entry.ptr))->x);
            break;
        case FieldType::Quat:
            ImGui::InputFloat4(label.c_str(), (float*)&((glm::quat*)(entry.ptr))->x);
            break;
        default:
            break;
    }
}

InspectorRegistry &EntityViewerPanel::inspector()
{
    return m_inspector;
}

InspectorRegistry::InspectorRegistry()
{
}

InspectorRegistry::~InspectorRegistry()
{
}

std::unordered_map<uint16_t, InspectorFn> &InspectorRegistry::registry()
{
    return m_registry;
}

