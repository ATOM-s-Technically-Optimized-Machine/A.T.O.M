#include <filesystem>
#include <memory>
#include <renderer/glad/glad.h>
#include <GLFW/glfw3.h>


#include "tools/Editor.hpp"

#include "core/ECS/Includes.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "inputs/ECSIntegration.hpp"
#include "renderer/RenderSystem.hpp"

#include <iostream>

using namespace editor;

Editor::Editor(GLFWwindow *window, ECS::ECS &ecs, RenderSystem &render, resources::ResourceManager &manager)
: m_ecs(ecs), m_render_sys(render), m_window(window)
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

void Editor::render()
{
    ImGui::Begin("Viewport");
    ImVec2 size = ImGui::GetContentRegionAvail();
    int w = (int)size.x, h = (int)size.y;

    if (w != m_fbo_w || h != m_fbo_h) {
        if (m_fbo) {
            glDeleteFramebuffers(1, &m_fbo);
            glDeleteTextures(1, &m_fbo_tex);
            glDeleteRenderbuffers(1, &m_fbo_depth);
        }

        m_fbo_w = w;
        m_fbo_h = h;
        genFBO(m_fbo_w, m_fbo_h, m_fbo, m_fbo_tex, m_fbo_depth);
        m_render_sys.setTargetFBO(m_fbo, m_fbo_w, m_fbo_h);
    }

    ImGui::Image((ImTextureID)(intptr_t)m_fbo_tex, size, ImVec2(0, 1), ImVec2(1, 0));

    bool hovered = ImGui::IsItemHovered();
    static bool captured = false;

    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        captured = true;
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        if (captured) {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        captured = false;
    }

    std::vector<ECS::EntityID> cameras;
    m_ecs.getEntitiesByComponentsAllOf<Atom::Input::CameraComponent>(cameras);
    ECS::EntityID cam_id = cameras[0];
    auto &cam = m_ecs.entityGetComponent<Atom::Input::CameraComponent>(cam_id);
    cam.enabled = captured ? true : hovered;

    ImGui::End();

    for (auto &win : m_sub_windows) {
        ImGui::Begin(win->name());
        win->render();
        if (ImGui::BeginPopupContextWindow("##ctx")) {
            win->contextMenu();
            ImGui::EndPopup();
        }
        ImGui::End();
    }
}

void Editor::renderDirTree(const std::filesystem::path &path)
{
    for (auto &entry : std::filesystem::directory_iterator(path)) {
        auto name = entry.path().filename().string();
        if (entry.is_directory()) {
            if (ImGui::TreeNode(name.c_str())) {
                renderDirTree(entry.path());
                ImGui::TreePop();
            }
        } else {
            ImGui::Text("%s", name.c_str());
        }
    }
}

void Editor::setAssetPath(const std::filesystem::path &path)
{
    m_assets_path = path;
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
