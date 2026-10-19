/*
** EPITECH PROJECT, 2026
** main
** File description:
** AssetTreeWindow
*/

#ifndef ASSETTREEWINDOW_HPP_
#define ASSETTREEWINDOW_HPP_

#include "tools/window/IWindow.hpp"
#include <filesystem>
#include <vector>

namespace editor {
    class Editor;
};

struct FileNode {
    std::string name;
    std::filesystem::path path;
    bool is_dir;
    std::vector<FileNode> children;
};

class AssetTreeWindow : public IWindow {
    public:
        AssetTreeWindow(editor::Editor &editor, const std::filesystem::path &root);
        ~AssetTreeWindow();

        const char *name() const override { return "AssetTreeWindow"; }
        void render() override;
        void contextMenu() override;
        
        void renderTree(const FileNode &node);
        static FileNode buildTree(const std::filesystem::path &path);

        void setAssetPath(const std::filesystem::path &path);

    protected:
    private:
        editor::Editor &m_editor;
        std::filesystem::path m_asset_path;
        FileNode m_root;
        bool m_should_refresh = true;
};

#endif /* !ASSETTREEWINDOW_HPP_ */
