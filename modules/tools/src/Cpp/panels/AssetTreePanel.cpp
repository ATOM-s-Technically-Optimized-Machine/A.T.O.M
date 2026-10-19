#include "tools/panels/AssetTreePanel.hpp"
#include "imgui.h"
#include "tools/panels/APanel.hpp"
#include <filesystem>

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
            ImGui::Text("%s", entry.name.c_str());
        }
    }
}

FileNode AssetTreePanel::buildTree(const std::filesystem::path &path)
{
    FileNode node;
    node.name = path.filename().string();
    node.path = path;
    node.is_dir = std::filesystem::is_directory(path);
    if (node.is_dir) {
        for (auto &entry : std::filesystem::directory_iterator(path)) {
            node.children.push_back(buildTree(entry.path()));
        }
    }
    return node;
}
