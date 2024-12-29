//
//  LveWindow.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include <stdexcept>

#include "LveWindow.hpp"

namespace Lve {

LveWindow::LveWindow(const int InWidth, const int InHeigth, const std::string InName)
: Width(InWidth)
, Heigth(InHeigth)
, Name(InName)
{
    InitWindow();
}

LveWindow::~LveWindow()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}

bool LveWindow::shouldClose()
{
    return glfwWindowShouldClose(Window);
}

void LveWindow::createWindowSurface(VkInstance Instance, VkSurfaceKHR* Surface)
{
    if (glfwCreateWindowSurface(Instance, Window, nullptr, Surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }
    
}

void LveWindow::InitWindow()
{
    // Initialize library
    glfwInit();
    
    // Ask to not create a OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    // Avoid resizing
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    Window = glfwCreateWindow(Width, Heigth, Name.c_str(), nullptr, nullptr);
}

}
