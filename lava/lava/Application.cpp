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

void Application::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pSetLayouts = nullptr;
    PipelineLayoutInfo.pushConstantRangeCount = 0;
    PipelineLayoutInfo.pPushConstantRanges = nullptr;
    
    if (vkCreatePipelineLayout(Device.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void Application::CreatePipeline()
{
    LvePipelineConfigInfo PipelineConfigInfo = LvePipeline::defaultPipelineConfigInfo(SwapChain.width(), SwapChain.height());
    
    // Basically the render pass says to the graphics pipeline what kind of output to create
    // (meaning how color buffer, depth etc. are allocated in the frame buffer)
    PipelineConfigInfo.renderPass = SwapChain.getRenderPass();
    PipelineConfigInfo.pipelineLayout = PipelineLayout;
    Pipeline = std::make_unique<LvePipeline>(Device, PipelineConfigInfo, absPathPrefix+"shaders/vertex_shader.vert.spv", absPathPrefix+"shaders/fragment_shader.frag.spv");
}

void Application::CreateCommandBuffers()
{
    
}

}
