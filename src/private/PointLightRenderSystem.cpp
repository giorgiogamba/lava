//
//  RenderSystem.cpp
//  lava
//
//  Created by Giorgio Gamba on 10/01/25.
//

#include "PointLightRenderSystem.hpp"

#define GLM_FORCE_RADIANS // expects angles to be defined in radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <filesystem>

namespace lava {

#pragma region Lifecycle

PointLightRenderSystem::PointLightRenderSystem(LavaDevice& InDevice, VkRenderPass InRenderPass, VkDescriptorSetLayout GlobalSetLayout)
: Device(InDevice)
{
    CreatePipelineLayout(GlobalSetLayout);
    CreatePipeline(InRenderPass);
}

PointLightRenderSystem::~PointLightRenderSystem()
{
    vkDestroyPipelineLayout(Device.device(), PipelineLayout, nullptr);
}

#pragma endregion

#pragma region Pipeline

void PointLightRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout GlobalSetLayout) 
{
    // VkPushConstantRange PushConstantRange{};
    // PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // PushConstantRange.offset = 0;
    // PushConstantRange.size = sizeof(PushConstantRange);

    std::vector<VkDescriptorSetLayout> DescriptorSetLayouts{GlobalSetLayout};
    
    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
    PipelineLayoutInfo.pSetLayouts = DescriptorSetLayouts.data();
    PipelineLayoutInfo.pushConstantRangeCount = 0;
    PipelineLayoutInfo.pPushConstantRanges = nullptr;
    
    if (vkCreatePipelineLayout(Device.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void PointLightRenderSystem::CreatePipeline(VkRenderPass& RenderPass)
{
    assert(PipelineLayout && "Pipeline Layout is null");
    
    LavaPipelineConfigInfo PipelineConfigInfo;
    LavaPipeline::defaultPipelineConfigInfo(PipelineConfigInfo);
    
    // Basically the render pass says to the graphics pipeline what kind of output to create
    // (meaning how color buffer, depth etc. are allocated in the frame buffer)
    PipelineConfigInfo.renderPass = RenderPass;
    PipelineConfigInfo.pipelineLayout = PipelineLayout;
    const std::filesystem::path vertexShaderAbsPath = std::filesystem::absolute("shaders/point_light_shader.vert.spv");
    const std::filesystem::path fragmentShaderAbsPath = std::filesystem::absolute("shaders/point_light_shader.frag.spv");
    Pipeline = std::make_unique<LavaPipeline>(Device, PipelineConfigInfo, vertexShaderAbsPath, fragmentShaderAbsPath);
}

#pragma endregion

#pragma region GameObjects


void PointLightRenderSystem::RenderGameObjects(const FrameDescriptor& FrameDesc)
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
    
        // 6 because of the vertices of the 2 triangle composing the gizmo
    vkCmdDraw(FrameDesc.CommandBuffer, 6, 1, 0, 0);
}

#pragma endregion

}

