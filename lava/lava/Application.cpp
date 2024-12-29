//
//  Application.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include "Application.hpp"

namespace Lve {

Application::Application()
{
    CreatePipelineLayout();
    CreatePipeline();
    CreateCommandBuffers();
}

Application::~Application()
{
    vkDestroyPipelineLayout(Device.device(), PipelineLayout, nullptr);
}

void Application::Run()
{
    while (!Window.shouldClose())
    {
        glfwPollEvents();
    }
}

}
