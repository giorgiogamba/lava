//
//  LavaWindow.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include <stdexcept>

#include "LavaWindow.hpp"

#include <GLFW/glfw3.h>

namespace Lava {

LavaWindow::LavaWindow(const std::string InName, const int InWidth, const int InHeigth)
: Name(InName)
, Width(InWidth)
, Heigth(InHeigth)
, bFrameBufferResized(false)
{
    InitWindow();
}

LavaWindow::~LavaWindow()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}

bool LavaWindow::shouldClose()
{
    return glfwWindowShouldClose(Window);
}

void LavaWindow::createWindowSurface(VkInstance Instance, VkSurfaceKHR* Surface)
{
    if (glfwCreateWindowSurface(Instance, Window, nullptr, Surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }
    
}

void LavaWindow::InitWindow()
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

void LavaWindow::FrameBufferResizedCallback(GLFWwindow* InWindow, const int Width, const int Height)
{
    auto window = reinterpret_cast<LavaWindow*>(glfwGetWindowUserPointer(InWindow));
    window->bFrameBufferResized = true;
    window->Width = Width;
    window->Heigth = Height;
}

}
