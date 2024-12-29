//
//  Application.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include "Application.hpp"

namespace Lve {

void Application::Run()
{
    while (!Window.shouldClose())
    {
        glfwPollEvents();
    }
}

}
