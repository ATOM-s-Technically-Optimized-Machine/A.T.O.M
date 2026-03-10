#include "renderer/GLFWInputProvider.hpp"
#include "renderer/Input.hpp"
#include <GLFW/glfw3.h>

GLFWInputProvider::GLFWInputProvider(GLFWwindow *window)
: m_window(window)
{
}

void GLFWInputProvider::fetch(InputState &state)
{
    static const int keyMap[(int)Key::_Count] = {
        GLFW_KEY_W,
        GLFW_KEY_A,
        GLFW_KEY_S,
        GLFW_KEY_D,
        GLFW_KEY_SPACE,
        GLFW_KEY_LEFT_SHIFT,
        GLFW_KEY_ESCAPE,
    };

    for (int i = 0; i < (int)Key::_Count; i++) {
        state.keys[i] = glfwGetKey(m_window, keyMap[i]) == GLFW_PRESS;
    }

    double mx, my;
    glfwGetCursorPos(m_window, &mx, &my);

    if (m_first) {
        m_lastX = mx;
        m_lastY = my;
        m_first = false;
    }

    state.mouseDx = (float)(mx - m_lastX);
    state.mouseDy = (float)(my - m_lastY);
    m_lastX = mx;
    m_lastY = my;
}
