#ifndef CUSTOMIZEEDITORPANEL_HPP_
    #define CUSTOMIZEEDITORPANEL_HPP_

#include "imgui.h"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"
class CustomizeEditorPanel : public APanel {
    public:
        CustomizeEditorPanel(editor::Editor &editor);
        ~CustomizeEditorPanel();

        void render(int view_id) override;
    protected:
    private:
        ImGuiStyle m_style;
};

#endif /* !CUSTOMIZEEDITORPANEL_HPP_ */
