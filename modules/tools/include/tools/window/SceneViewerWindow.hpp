/*
** EPITECH PROJECT, 2026
** main
** File description:
** SceneViewerWindow
*/

#ifndef SCENEVIEWERWINDOW_HPP_
#define SCENEVIEWERWINDOW_HPP_

#include "tools/window/IWindow.hpp"

class SceneViewerWindow : public IWindow {
    public:
        SceneViewerWindow();
        ~SceneViewerWindow();

        const char *name() const override { return "SceneViewerWindow"; };
        void render() override;
        void contextMenu() override;

    protected:
    private:
};

#endif /* !SCENEVIEWERWINDOW_HPP_ */
