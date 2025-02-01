//
//  RenderSystem.cpp
//  lava
//
//  Created by Giorgio Gamba on 10/01/25.
//

#include "RenderSystem.hpp"

#define GLM_FORCE_RADIANS // expects angles to be defined in radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <filesystem>

namespace lava {

#pragma region Lifecycle

RenderSystem::RenderSystem(LavaDevice& InDevice, VkRenderPass InRenderPass)
: Device(InDevice)
{
    CreatePipelineLayout();
    CreatePipeline(InRenderPass);
}

RenderSystem::~RenderSystem()
{
    vkDestroyPipelineLayout(Device.device(), PipelineLayout, nullptr);
}

#pragma endregion

#pragma region Pipeline

void RenderSystem::CreatePipelineLayout()
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

void RenderSystem::CreatePipeline(VkRenderPass& RenderPass)
{
    assert(PipelineLayout && "Pipeline Layout is null");
    
    LavaPipelineConfigInfo PipelineConfigInfo;
    LavaPipeline::defaultPipelineConfigInfo(PipelineConfigInfo);
    
    // Basically the render pass says to the graphics pipeline what kind of output to create
    // (meaning how color buffer, depth etc. are allocated in the frame buffer)
    PipelineConfigInfo.renderPass = RenderPass;
    PipelineConfigInfo.pipelineLayout = PipelineLayout;
    const std::filesystem::path vertexShaderAbsPath = std::filesystem::absolute("shaders/vertex_shader.vert.spv");
    const std::filesystem::path fragmentShaderAbsPath = std::filesystem::absolute("shaders/fragment_shader.frag.spv");
    Pipeline = std::make_unique<LavaPipeline>(Device, PipelineConfigInfo, vertexShaderAbsPath, fragmentShaderAbsPath);
}

#pragma endregion

#pragma region GameObjects


void RenderSystem::RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<LavaGameObject>& GameObjects, const LavaCamera& Camera)
{
    Pipeline->Bind(CommandBuffer);
    
    auto ProjectionView = Camera.GetProjectionMat() * Camera.GetViewMat();
    
    for (LavaGameObject& GameObject : GameObjects)
    {
        PushConstant3DData PushConstant{};
        //PushConstant.color = GameObject.GetColor();
        
        glm::mat4 modelMatrix = GameObject.Transform.mat4();
        PushConstant.transform = ProjectionView * modelMatrix;
        PushConstant.modelMatrix = modelMatrix;

        // Applies perspective
        PushConstant.transform = ProjectionView * GameObject.Transform.mat4();
        
        vkCmdPushConstants(CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstant3DData), &PushConstant);
        
        GameObject.GetModel()->Bind(CommandBuffer);
        GameObject.GetModel()->Draw(CommandBuffer);
    }
}

#pragma endregion

}

