//
//  LveWindow.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

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

void LveWindow::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    Window = glfwCreateWindow(Width, Heigth, Name.c_str(), nullptr, nullptr);
}

}
