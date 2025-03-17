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

RenderSystem::RenderSystem(LavaDevice& InDevice, VkRenderPass InRenderPass, VkDescriptorSetLayout GlobalSetLayout)
: Device(InDevice)
{
    CreatePipelineLayout(GlobalSetLayout);
    CreatePipeline(InRenderPass);
}

RenderSystem::~RenderSystem()
{
    vkDestroyPipelineLayout(Device.device(), PipelineLayout, nullptr);
}

#pragma endregion

#pragma region Pipeline

void RenderSystem::CreatePipelineLayout(VkDescriptorSetLayout GlobalSetLayout)
{
    VkPushConstantRange PushConstantRange{};
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(PushConstantRange);

    std::vector<VkDescriptorSetLayout> DescriptorSetLayouts{GlobalSetLayout};
    
    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
    PipelineLayoutInfo.pSetLayouts = DescriptorSetLayouts.data();
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


void RenderSystem::RenderGameObjects(const FrameDescriptor& FrameDesc, std::vector<LavaGameObject>& GameObjects)
{
    Pipeline->Bind(FrameDesc.CommandBuffer);

    // At each frame we can bind multiple sets at time, but you must point the starting set
    // also if you are adding a set in previous positions
    vkCmdBindDescriptorSets
        ( FrameDesc.CommandBuffer
        , VK_PIPELINE_BIND_POINT_GRAPHICS
        , PipelineLayout
        , 0
        , 1
        , &FrameDesc.GlobalDescriptorSet
        , 0
        , nullptr);
    
    for (LavaGameObject& GameObject : GameObjects)
    {
        PushConstant3DData PushConstant{};       
        PushConstant.ModelMatrix = GameObject.Transform.mat4();
        PushConstant.normalMatrix = GameObject.Transform.normalMatrix(); // Automatically padded to 4x4 matrix
        
        vkCmdPushConstants(FrameDesc.CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstant3DData), &PushConstant);
        
        const std::shared_ptr<LavaModel> Model = GameObject.GetModel();
        if (Model)
        {
            Model->Bind(FrameDesc.CommandBuffer);
            Model->Draw(FrameDesc.CommandBuffer);
        }
    }
}

#pragma endregion

}

