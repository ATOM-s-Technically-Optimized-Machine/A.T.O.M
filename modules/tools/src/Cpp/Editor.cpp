#include <cfloat>
#include <cstdio>
#include <renderer/glad/glad.h>
#include <GLFW/glfw3.h>


#include "tools/Editor.hpp"

#include "core/ECS/Includes.hpp"
#include "inputs/ECSIntegration.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <utility>

using namespace editor;

Editor::Editor(GLFWwindow *window, ECS::ECS &ecs, resources::ResourceManager &manager)
: m_ecs(ecs), m_window(window), m_manager(manager)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

Editor::~Editor()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::setCursorCaptured(bool captured)
{
    if (captured == m_cursor_captured)
        return;
    m_cursor_captured = captured;
    m_ecs.getSystem<Atom::Input::InputECSSystem>().setCursorCaptured(captured);

    if (!captured) {
        // GLFW restores the cursor without firing a position callback,
        // so ImGui would keep its off-screen position until the mouse moves.
        double mx, my;
        glfwGetCursorPos(m_window, &mx, &my);
        ImGui::GetIO().AddMousePosEvent((float)mx, (float)my);
    }
}

void Editor::beginFrame()
{
    if (m_cursor_captured)
        ImGui::GetIO().AddMousePosEvent(-FLT_MAX, -FLT_MAX);
}

void Editor::render()
{
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_P)) {
        m_palette_open = true;
        m_palette_buff[0] = '\0';
    }

    if (m_palette_open) commandPalette();

    std::vector<ViewEntry> to_close;

    for (auto &[id, view] : m_views) {
        auto panel = view.panel.lock();
        bool success = ImGui::Begin(std::string(view.name + "##" + std::to_string(id)).c_str(), &view.is_open);
        if (!success) {
            ImGui::End();
            continue;
        }
        bool captures = panel->capturesMouse();
        panel->render(id);
        if (!captures && ImGui::BeginPopupContextWindow("##ctx")) {
            panel->contextMenu(id);
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    for (auto &[id, view] : m_views)
        if (!view.is_open)
            to_close.push_back(view);

    for (auto &view : to_close)
        killView(view.id);
}

void Editor::commandPalette()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0));
    ImGui::Begin("##palette", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize
    );

    ImGui::Text("Quick command");

    if (ImGui::IsWindowAppearing()) {
        ImGui::SetKeyboardFocusHere();
        m_palette_selected = 0;
    }

    bool text_changed = ImGui::InputText("##query", m_palette_buff, sizeof(m_palette_buff));
    if (text_changed) m_palette_selected = 0;

    std::string query(m_palette_buff);
    std::transform(query.begin(), query.end(), query.begin(), [](unsigned char c) { return std::tolower(c); });

    std::vector<std::pair<int, std::size_t>> filtered;
    for (int i = 0; i < (int)m_registry.size(); i++) {
        std::size_t idx = m_registry[i].lowercase_name.find(query);
        if (idx != std::string::npos)
            filtered.push_back(std::move(std::make_pair(i, idx)));
    }
    
    if (!filtered.empty())
        std::sort(filtered.begin(), filtered.end(), [](auto &a, auto &b) { return a.second < b.second; });

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) m_palette_selected++;
    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) m_palette_selected--;
    m_palette_selected = std::clamp(m_palette_selected, 0, std::max(0, (int)filtered.size() - 1));

    for (int i = 0; i < filtered.size(); i++) {
        bool sel = m_palette_selected == i;
        if (ImGui::Selectable(m_registry[filtered[i].first].name.c_str(), sel)
                || (ImGui::IsKeyPressed(ImGuiKey_Enter) && sel)) {
            spawnView(m_registry[filtered[i].first].name);
            m_palette_open = false;
            m_palette_buff[0] = '\0';
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape)) {
        m_palette_open = false;
        m_palette_buff[0] = '\0';
    }
    ImGui::End();
}

int Editor::spawnView(const std::string &name)
{
    for (auto &entry : m_registry) {
        if (entry.name == name) {
            if (entry.unique_view && getViews(name).size() != 0) {
                std::cerr << "Cannot spawn view. Panel is marked as 'unique' and a view to this panel already exists." << std::endl;
                return -1;
            }
            int view_id = m_ids++;
            m_views[view_id] = {
                .id = (std::size_t)view_id,
                .is_open = true,
                .name = entry.name,
                .panel = entry.panel
            };
            return view_id;
        }
    }
    return -1;
}

void Editor::killView(int id)
{
    auto it = m_views.find(id);
    if (it == m_views.end())
        return;
    it->second.panel.lock()->onViewClose(id);
    m_views.erase(it);
}

std::vector<int> Editor::getViews(const std::string &name)
{
    std::vector<int> ids;

    for (auto &entry : m_views) {
        if (entry.second.name == name)
            ids.push_back(entry.second.id);
    }
    return ids;
}

std::shared_ptr<IPanel> Editor::getPanel(const std::string &name)
{
    const auto &ref = std::find_if(m_registry.begin(), m_registry.end(), [name](PanelRegistryEntry &entry) { return entry.name == name; });
    return ref != m_registry.end() ? ref->panel : nullptr;
}

void editor::genFBO(
    int width,
    int height,
    unsigned int &out_fbo,
    unsigned int &out_texture,
    unsigned int &out_depth
)
{
    glGenFramebuffers(1, &out_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, out_fbo);
    
    glGenTextures(1, &out_texture);
    glBindTexture(GL_TEXTURE_2D, out_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_texture, 0);

    glGenRenderbuffers(1, &out_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, out_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, out_depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "FBO incomplete: 0x" << std::hex << status << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
