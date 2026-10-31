#include "tools/panels/AssetTreePanel.hpp"
#include "imgui.h"
#include "tools/panels/APanel.hpp"
#include "resources/ResourceIncludes.hpp"
#include "tools/Editor.hpp"
#include <filesystem>
#include <iostream>
#include <core/DefaultComponents.hpp>
#include "inputs/ECSIntegration.hpp"

AssetTreePanel::AssetTreePanel(editor::Editor &editor, const std::filesystem::path &root)
: APanel(editor), m_asset_path(root)
{
    m_name = "Assets";
}

AssetTreePanel::~AssetTreePanel()
{
}

void AssetTreePanel::render(int view_id)
{
    if (m_should_refresh) {
        if (std::filesystem::exists(m_asset_path))
            m_root = buildTree(m_asset_path);
        m_should_refresh = false;
    }
    renderTree(m_root);
}

void AssetTreePanel::contextMenu(int view_id)
{
    if (ImGui::MenuItem("Refresh"))
        m_should_refresh = true;
}

void AssetTreePanel::renderTree(const FileNode &node)
{
    for (auto &entry : node.children) {
        if (entry.is_dir) {
            if (ImGui::TreeNode(entry.name.c_str())) {
                renderTree(entry);
                ImGui::TreePop();
            }
        } else {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

            if (entry.path == m_selected_asset)
                flags |= ImGuiTreeNodeFlags_Selected;
            ImGui::TreeNodeEx(entry.name.c_str(), flags);
            if (ImGui::IsItemClicked())
                m_selected_asset = entry.path;
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                spawnAsset(entry);
            }
            // Drag'n'drop ?
        }
    }
}

FileNode AssetTreePanel::buildTree(const std::filesystem::path &path)
{
    resources::AssetRegistry &registry = m_editor.getResourceManager().assets();

    FileNode node;
    node.name = path.filename().string();
    node.path = path;
    node.is_dir = std::filesystem::is_directory(path);
    if (node.is_dir) {
        for (auto &entry : std::filesystem::directory_iterator(path)) {
            if (
                !std::filesystem::is_directory(entry.path())
                && registry.classify(entry.path()) == resources::AssetKind::Unknown
            )
                continue;
            node.children.push_back(buildTree(entry.path()));
        }
    }
    return node;
}

void AssetTreePanel::spawnAsset(const FileNode &entry)
{
    resources::ResourceManager &manager = m_editor.getResourceManager();
    resources::AssetRegistry &registry = manager.assets();

    if (registry.classify(entry.path) != resources::AssetKind::Mesh)
        return; // Currently no point in trying to spawn anything else

    resources::ResourceID mesh_id = manager.load<resources::MeshData>(entry.path);

    // Creates a new VAO on every call, not ideal, but fine for now
    MeshHandleComponent comp = MeshHandleComponent::fromMeshData(manager.get<resources::MeshData>(mesh_id));

    ECS::ECS &ecs = m_editor.getECS();
    std::vector<ECS::EntityID> cameras;
    ecs.getEntitiesByComponentsAllOf<Atom::Input::CameraComponent>(cameras);
    Atom::Input::CameraComponent &cam = ecs.entityGetComponent<Atom::Input::CameraComponent>(cameras[0]);

    glm::vec3 spawn_position = cam.camera.position() + cam.camera.forward() * 5.0f; // hardcoded 5.0 distance

    ECS::EntityID e = ecs.entityCreate();

    ecs.entityAddComponent<TransformComponent>(e).position = spawn_position;
    ecs.entityAddComponent<MeshHandleComponent>(e) = comp;
    ecs.entityAddComponent<NameComponent>(e) = entry.name;
    ecs.entityAddComponent<HierarchyNode>(e).parent = 0;
    ecs.entityGetComponent<HierarchyNode>(0).children.push_back(e);
}
