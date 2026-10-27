#include "tools/panels/EntityHierarchyPanel.hpp"
#include "core/DefaultComponents.hpp"
#include "core/ECS/Component.hpp"
#include "core/ECS/Includes.hpp"
#include "imgui.h"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"
#include "tools/panels/EntityViewerPanel.hpp"
#include <algorithm>
#include <vector>

EntityHierarchyPanel::EntityHierarchyPanel(editor::Editor &editor, ECS::ECS &ecs)
: APanel(editor), m_ecs(ecs)
{
}

EntityHierarchyPanel::~EntityHierarchyPanel()
{
}

void EntityHierarchyPanel::render(int view_id)
{
    auto &pool = m_ecs.getPool<HierarchyNode>();
    auto &dense = pool.getData();

    std::vector<ECS::EntityID> roots;

    for (std::size_t i = 0; i < dense.entities.size(); ++i) {
        if (dense.components[i].parent == ECS::NULL_ENTITY)
            roots.push_back(dense.entities[i]);
    }

    if (roots.size() == 0) {
        ImGui::Text("Couldn't build hierarchy tree: No root entity");
        return;
    }

    for (auto entity : roots) {
        renderEntities(dense, entity);
    }
}

void EntityHierarchyPanel::renderEntities(const ECS::DenseComponent<HierarchyNode> &data, ECS::EntityID curr_entity)
{
    const auto &it = std::find(data.entities.begin(), data.entities.end(), curr_entity);
    if (it == data.entities.end())
        return;
    const HierarchyNode &node = data.components[std::distance(data.entities.begin(), it)];
    std::string display_name =
        m_ecs.entityHasComponent<NameComponent>(curr_entity)
        ? m_ecs.entityGetComponent<NameComponent>(curr_entity) + " (" + std::to_string(curr_entity) + ")"
        : "Entity ID " + std::to_string(curr_entity);

    ECS::EntityID selected = m_editor.getSelectedEntity();

    if (node.children.size() != 0) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (curr_entity == selected)
            flags |= ImGuiTreeNodeFlags_Selected;
        bool open = ImGui::TreeNodeEx(display_name.c_str(), flags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            m_editor.setSelectedEntity(curr_entity);
        if (open) {
            for (auto &other : node.children)
                renderEntities(data, other);
            ImGui::TreePop();
        }
    } else {
        if (ImGui::Selectable(display_name.c_str(), curr_entity == selected))
            m_editor.setSelectedEntity(curr_entity);
    }
}
