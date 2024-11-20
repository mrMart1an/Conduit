#include "window/glfw/glfwWindow.h"

#include <GLFW/glfw3.h>

namespace cndt::glfw {

// Swap buffer for the OpenGL context
void GlfwWindow::glSwapBuffer()
{
    glfwSwapBuffers(m_glfw_window);
}

// Set OpenGL v-sync settings
void GlfwWindow::glSetVSync(bool v_sync)
{
    glfwSwapInterval(v_sync ? 1 : 0);
}

} // namespace cndt::glfw
