/*
** EPITECH PROJECT, 2026
** main
** File description:
** CustomizeEditorPanel
*/

#include "tools/panels/CustomizeEditorPanel.hpp"
#include "imgui.h"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"

CustomizeEditorPanel::CustomizeEditorPanel(editor::Editor &editor)
: APanel(editor)
{
}

CustomizeEditorPanel::~CustomizeEditorPanel()
{
}

void CustomizeEditorPanel::render(int view_id)
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::ShowStyleEditor(&style);
}
