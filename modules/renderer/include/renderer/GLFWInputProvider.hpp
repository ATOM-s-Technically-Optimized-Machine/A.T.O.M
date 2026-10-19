#ifndef GLFWINPUTPROVIDER_HPP_
    #define GLFWINPUTPROVIDER_HPP_

    #include <GLFW/glfw3.h>
    #include "renderer/Input.hpp"

class GLFWInputProvider : public IInputProvider {
    public:
        GLFWInputProvider(GLFWwindow *window);
        ~GLFWInputProvider() = default;

        void fetch(InputState &state) override;

    protected:
    private:
        GLFWwindow *m_window;
        double m_lastX = 0, m_lastY = 0;
        bool m_first = true;
};

#endif /* !GLFWINPUTPROVIDER_HPP_ */
