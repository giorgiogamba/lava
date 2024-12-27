//
//  LveWindow.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

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
    
    LveWindow(const int InWidth, const int InHeigth, const std::string InName);
    ~LveWindow();
    
private:
    
    void InitWindow();
    
    int Width;
    int Heigth;
    std::string Name;
    
    GLFWwindow* Window;
    
};
}
