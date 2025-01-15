//
//  LveWindow.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include <stdexcept>

#include "LveWindow.hpp"

#include <GLFW/glfw3.h>

namespace Lve {

LveWindow::LveWindow(const std::string InName, const int InWidth, const int InHeigth)
: Name(InName)
, Width(InWidth)
, Heigth(InHeigth)
, bFrameBufferResized(false)
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    Window = glfwCreateWindow(Width, Heigth, Name.c_str(), nullptr, nullptr);
    
    glfwSetWindowUserPointer(Window, this); // Sets to a custom pointer
    glfwSetFramebufferSizeCallback(Window, FrameBufferResizedCallback);
}

void LveWindow::FrameBufferResizedCallback(GLFWwindow* InWindow, const int Width, const int Height)
{
    auto window = reinterpret_cast<LveWindow*>(glfwGetWindowUserPointer(InWindow));
    window->bFrameBufferResized = true;
    window->Width = Width;
    window->Heigth = Height;
}

}
