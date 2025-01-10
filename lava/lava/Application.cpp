//
//  Application.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include "Application.hpp"

#define GLM_FORCE_RADIANS // expects angles to be defined in radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Lve {

#pragma region Lifecycle

Application::Application()
{
    LoadGameObjects();
    CreatePipelineLayout();
    CreatePipeline();
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
        
        if (VkCommandBuffer CommandBuffer = Renderer.StartDrawFrame())
        {
            // We kept this operations divided in order to add other drawing elements in between
            
            Renderer.StartSwapChainRenderPass(CommandBuffer);
            RenderGameObjects(CommandBuffer);
            Renderer.EndSwapChainRenderPass(CommandBuffer);
            Renderer.EndDrawFrame();
        }
    }
    
    // Makes the CPU wait until all the GPU resources are freed
    vkDeviceWaitIdle(Device.device());
}

#pragma endregion

#pragma region Pipeline

void Application::CreatePipelineLayout()
{ 
    VkPushConstantRange PushConstantRange{};
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(PushConstantRange);
    
    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pSetLayouts = nullptr;
    PipelineLayoutInfo.pushConstantRangeCount = 1;
    PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;
    
    if (vkCreatePipelineLayout(Device.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void Application::CreatePipeline()
{
    assert(PipelineLayout && "Pipeline Layout is null");
    
    LvePipelineConfigInfo PipelineConfigInfo;
    LvePipeline::defaultPipelineConfigInfo(PipelineConfigInfo);
    
    // Basically the render pass says to the graphics pipeline what kind of output to create
    // (meaning how color buffer, depth etc. are allocated in the frame buffer)
    PipelineConfigInfo.renderPass = Renderer.GetSwapChainRenderPass();
    PipelineConfigInfo.pipelineLayout = PipelineLayout;
    Pipeline = std::make_unique<LvePipeline>(Device, PipelineConfigInfo, absPathPrefix+"shaders/vertex_shader.vert.spv", absPathPrefix+"shaders/fragment_shader.frag.spv");
}

#pragma endregion

#pragma region GameObjects

void Application::LoadGameObjects()
{
    // Initializes position and color for each vertex
    std::vector<Vertex> Vertices =
    { {{0.f, -0.5f}, {1.f, 0.f, 0.f}}
    , {{0.5f, 0.5f}, {0.f, 1.f, 0.f}}
    , {{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}};
    
    // Just define a single model that is shared between multiple game objects
    const std::shared_ptr<LveModel> Model = std::make_shared<LveModel>(Device, Vertices);
    
    LveGameObject Triangle = LveGameObject::CreateGameObject();
    Triangle.SetModel(Model);
    Triangle.SetColor({.1f, .8f, .1f});
    
    Triangle.Transform2D.Translation.x = .2f;
    Triangle.Transform2D.Scale = {2.f, .5f};
    Triangle.Transform2D.RotationAngle = .25f * glm::two_pi<float>();
    
    GameObjects.push_back(std::move(Triangle));
}


void Application::RenderGameObjects(VkCommandBuffer CommandBuffer)
{
    Pipeline->Bind(CommandBuffer);
    
    for (LveGameObject& GameObject : GameObjects)
    {
        PushConstantData PushConstant{};
        PushConstant.offset = GameObject.Transform2D.Translation;
        PushConstant.color = GameObject.GetColor();
        PushConstant.transform = GameObject.Transform2D.mat2();
        
        vkCmdPushConstants(CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &PushConstant);
        
        GameObject.GetModel()->Bind(CommandBuffer);
        GameObject.GetModel()->Draw(CommandBuffer);
    }
}

#pragma endregion

}
