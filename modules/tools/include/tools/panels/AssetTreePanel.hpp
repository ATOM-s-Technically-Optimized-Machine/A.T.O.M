#ifndef ASSETTREEPANEL_HPP_
#define ASSETTREEPANEL_HPP_

#include "tools/panels/APanel.hpp"
#include "tools/panels/IPanel.hpp"
#include <filesystem>
#include <vector>

struct FileNode {
    std::string name;
    std::filesystem::path path;
    bool is_dir;
    std::vector<FileNode> children;
};

class AssetTreePanel : public APanel {
    public:
        AssetTreePanel(editor::Editor &editor, const std::filesystem::path &root);
        ~AssetTreePanel();

        void render(int view_id) override;
        void contextMenu(int view_id) override;
        
        void renderTree(const FileNode &node);
        FileNode buildTree(const std::filesystem::path &path);

        void setAssetPath(const std::filesystem::path &path);

    protected:
    private:
        std::filesystem::path m_asset_path;
        FileNode m_root;
        bool m_should_refresh = true;
};

#endif /* !ASSETTREEPANEL_HPP_ */
