//
//  LveWindow.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

#ifndef LveWindow_hpp
#define LveWindow_hpp

#include <stdio.h>

#endif /* LveWindow_hpp */

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Lve
{

class LveWindow
{
    
public:
    
    LveWindow(const std::string InName, const int InWidth, const int InHeigth);
    ~LveWindow();
    
    // Delete copy constructor and operator to avoid dangling pointers,
    // becase we are usign pointers to GLFWwindow, which means that if we make a copy the LveWindow
    // and we delete one of them, we will also delete with Window pointer and thus
    // the remaining LveWindow will have a dangling pointer
    LveWindow(const LveWindow&) = delete;
    LveWindow& operator=(const LveWindow&) = delete;
    
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
