//
//  LavaWindow.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

#include <stdio.h>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lava
{

class LavaWindow
{
    
public:
    
    LavaWindow(const std::string InName, const int InWidth, const int InHeigth);
    ~LavaWindow();
    
    // Delete copy constructor and operator to avoid dangling pointers,
    // becase we are usign pointers to GLFWwindow, which means that if we make a copy the LavaWindow
    // and we delete one of them, we will also delete with Window pointer and thus
    // the remaining LavaWindow will have a dangling pointer
    LavaWindow(const LavaWindow&) = delete;
    LavaWindow& operator=(const LavaWindow&) = delete;
    
    bool shouldClose();
    
    void createWindowSurface(VkInstance Instance, VkSurfaceKHR* Surface);
    
    VkExtent2D getExtent() const { return { static_cast<uint32_t>(Width), static_cast<uint32_t>(Heigth) }; }
    
    bool WasWindowResized() const { return bFrameBufferResized; }
    void ResetWindowResizedStatus() { bFrameBufferResized = false; }
    
    GLFWwindow* GetGLFWwindow() const { return Window; }
    
private:
    
    void InitWindow();
    
    static void FrameBufferResizedCallback(GLFWwindow* InWindow, const int Width, const int Height);
    
    std::string Name;
    
    int Width;
    int Heigth;
    bool bFrameBufferResized;
    
    GLFWwindow* Window;
    
};

}
