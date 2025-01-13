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

namespace Lve {

#pragma region Lifecycle

RenderSystem::RenderSystem(LveDevice& InDevice, VkRenderPass InRenderPass)
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
    
    LvePipelineConfigInfo PipelineConfigInfo;
    LvePipeline::defaultPipelineConfigInfo(PipelineConfigInfo);
    
    // Basically the render pass says to the graphics pipeline what kind of output to create
    // (meaning how color buffer, depth etc. are allocated in the frame buffer)
    PipelineConfigInfo.renderPass = RenderPass;
    PipelineConfigInfo.pipelineLayout = PipelineLayout;
    Pipeline = std::make_unique<LvePipeline>(Device, PipelineConfigInfo, absPathPrefix+"shaders/vertex_shader.vert.spv", absPathPrefix+"shaders/fragment_shader.frag.spv");
}

#pragma endregion

#pragma region GameObjects


void RenderSystem::RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<LveGameObject>& GameObjects)
{
    Pipeline->Bind(CommandBuffer);
    
    for (LveGameObject& GameObject : GameObjects)
    {
        // Make the cube rotate
        GameObject.Transform.Rotation.y = glm::mod(GameObject.Transform.Rotation.y + 0.01f, glm::two_pi<float>());
        GameObject.Transform.Rotation.x = glm::mod(GameObject.Transform.Rotation.x + 0.005f, glm::two_pi<float>());
        
        PushConstant3DData PushConstant{};
        PushConstant.color = GameObject.GetColor();
        PushConstant.transform = GameObject.Transform.mat4();
        
        vkCmdPushConstants(CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstant3DData), &PushConstant);
        
        GameObject.GetModel()->Bind(CommandBuffer);
        GameObject.GetModel()->Draw(CommandBuffer);
    }
}

#pragma endregion

}

