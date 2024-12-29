//
//  LvePipeline.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include <fstream>
#include <iostream>

#include "LvePipeline.hpp"
#include "LveDevice.hpp"

namespace Lve {

LvePipelineConfigInfo LvePipeline::defaultPipelineConfigInfo(const uint32_t Width, const uint32_t Height)
{
    LvePipelineConfigInfo ConfigInfo{};
    
    // Add Pipelines configurations
    
    // Input assembly
    // Takes in input coordinates grouped in geometries, specified in topology
    ConfigInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ConfigInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ConfigInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
    
    // Viewport configuration
    ConfigInfo.viewport.x = 0.f;
    ConfigInfo.viewport.y = 0.f;
    ConfigInfo.viewport.width = static_cast<float>(Width);
    ConfigInfo.viewport.height = static_cast<float>(Height);
    ConfigInfo.viewport.minDepth = 0.f;
    ConfigInfo.viewport.maxDepth = 1.f;
    
    // Defines the rendered area
    ConfigInfo.scissor.offset = {0, 0};
    ConfigInfo.scissor.extent = {Width, Height};
    
    // Rasterization stage
    ConfigInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    ConfigInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    ConfigInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    
    ConfigInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    ConfigInfo.rasterizationInfo.lineWidth = 1.f;
    
    ConfigInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    ConfigInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    
    ConfigInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    ConfigInfo.rasterizationInfo.depthBiasConstantFactor = 0.f;
    ConfigInfo.rasterizationInfo.depthBiasClamp = 0.f;
    ConfigInfo.rasterizationInfo.depthBiasSlopeFactor = 0.f;
    
    // Multisample
    // Avoids alising and jagging
    ConfigInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ConfigInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    ConfigInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ConfigInfo.multisampleInfo.minSampleShading = 1.f;
    ConfigInfo.multisampleInfo.pSampleMask = nullptr;
    ConfigInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    ConfigInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;
    
    ConfigInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT;
    ConfigInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    ConfigInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    ConfigInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    ConfigInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    ConfigInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ConfigInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ConfigInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    ConfigInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ConfigInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    ConfigInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    ConfigInfo.colorBlendInfo.attachmentCount = 1;
    ConfigInfo.colorBlendInfo.pAttachments = &ConfigInfo.colorBlendAttachment;
    ConfigInfo.colorBlendInfo.blendConstants[0] = 0.f;
    ConfigInfo.colorBlendInfo.blendConstants[1] = 0.f;
    ConfigInfo.colorBlendInfo.blendConstants[2] = 0.f;
    ConfigInfo.colorBlendInfo.blendConstants[3] = 0.f;
    
    ConfigInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ConfigInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    ConfigInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    ConfigInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    ConfigInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    ConfigInfo.depthStencilInfo.minDepthBounds = 0.f;
    ConfigInfo.depthStencilInfo.maxDepthBounds = 1.f;
    ConfigInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    ConfigInfo.depthStencilInfo.front = {};
    ConfigInfo.depthStencilInfo.back = {};
    
    return ConfigInfo;
}

LvePipeline::LvePipeline(LveDevice& InDevice, const LvePipelineConfigInfo& configInfo, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
: Device(InDevice)
{
    createPipeline(configInfo, vertexShaderPath, fragmentShaderPath);
}

std::vector<char> LvePipeline::readFile(const std::string& filePath)
{
    // ate makes the file stream seek to the end immediately
    // binary makes the file read as binary (because we already compiled it)
    std::ifstream filestream(filePath, std::ios::ate | std::ios::binary);
    
    if (!filestream.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    
    size_t size = static_cast<size_t>(filestream.tellg());
    std::vector<char> buffer(size);
    
    filestream.seekg(0);
    filestream.read(buffer.data(), size);
    
    filestream.close();
    
    return buffer;
}

void LvePipeline::createPipeline(const LvePipelineConfigInfo& configInfo, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    const std::vector<char>& vertexShaderCode = readFile(vertexShaderPath);
    const std::vector<char>& fragmentShaderCode = readFile(fragmentShaderPath);
    
    std::cout << "Shaders allocation completed" << "\n";
    std::cout << "Vertex shader size: " << vertexShaderCode.size() << "\n";
    std::cout << "Fragment shader size: " << fragmentShaderCode.size() << std::endl;
    
    createShaderModule(vertexShaderCode, &vertexShaderModule);
    createShaderModule(fragmentShaderCode, &fragmentShaderModule);
    
    VkPipelineShaderStageCreateInfo ShaderStages[2];
    
    ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    ShaderStages[0].module = vertexShaderModule;
    ShaderStages[0].pName = "main";
    ShaderStages[0].flags = 0;
    ShaderStages[0].pNext = nullptr;
    ShaderStages[0].pSpecializationInfo = nullptr;
    
    ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ShaderStages[1].module = fragmentShaderModule;
    ShaderStages[1].pName = "main";
    ShaderStages[1].flags = 0;
    ShaderStages[1].pNext = nullptr;
    ShaderStages[1].pSpecializationInfo = nullptr;
    
    VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputInfo.vertexAttributeDescriptionCount = 0;
    VertexInputInfo.vertexBindingDescriptionCount = 0;
    VertexInputInfo.pVertexAttributeDescriptions = nullptr;
    VertexInputInfo.pVertexBindingDescriptions = nullptr;
    
    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = 2;
    PipelineInfo.pStages = ShaderStages;
    PipelineInfo.pVertexInputState = &VertexInputInfo;
    PipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
    PipelineInfo.pViewportState = &configInfo.viewportInfo;
    PipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
    PipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
    PipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
    PipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
    PipelineInfo.pDynamicState = nullptr;
    
    PipelineInfo.layout = configInfo.pipelineLayout;
    PipelineInfo.renderPass = configInfo.renderPass;
    PipelineInfo.subpass = configInfo.subpass;
    
    PipelineInfo.basePipelineIndex = -1;
    PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    if (vkCreateGraphicsPipelines(Device.device(), VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &Pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Graphics pipeline");
    }
}

void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* Module)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    
    // Since code dara are stored in a vector, its default allocat
    // already ensures correctness by the worst case alignment requirement
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    if (vkCreateShaderModule(Device.device(), &createInfo, nullptr, Module) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module");
    }
}


}
