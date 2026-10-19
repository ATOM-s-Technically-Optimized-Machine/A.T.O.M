#ifndef APANEL_HPP_
    #define APANEL_HPP_

#include "tools/panels/IPanel.hpp"
#include <string>
#include <vector>

class APanel : public IPanel {
    public:
        APanel(editor::Editor &editor) : m_editor(editor) {};
        ~APanel() {};

        const char *name() const override { return m_name.c_str(); };
        void render(int view_id) override { return; };
        void contextMenu(int view_id) override { return; };
        bool capturesMouse() const override { return m_captured; };
        void onViewClose(int view_id) override { return; };
    protected:
        editor::Editor &m_editor;
        bool m_captured = false;
        std::string m_name = "panel";
        std::vector<int> m_view_ids;
    private:
};

#endif /* !APANEL_HPP_ */
