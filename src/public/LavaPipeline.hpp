//
//  LavaPipeline.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

#ifndef LavaPipeline_hpp
#define LavaPipeline_hpp

#include <stdio.h>
#include <string>
#include <vector>

#include "LavaDevice.hpp"

#include <vulkan/vulkan.h>

#endif /* LavaPipeline_hpp */

namespace lava {

#pragma region Types

struct LavaPipelineConfigInfo
{
    LavaPipelineConfigInfo() = default;
    
    LavaPipelineConfigInfo(const LavaPipelineConfigInfo&) = delete;
    LavaPipelineConfigInfo& operator=(LavaPipelineConfigInfo&) = delete;
    
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    
    // Use dynamic viewport so that dynamic pipeline is no longer dependent on swapchain dimension
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

#pragma endregion

class LavaPipeline
{
public:
    
    LavaPipeline(LavaDevice& InDevice, const LavaPipelineConfigInfo& configInfo, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~LavaPipeline();
    
    LavaPipeline(const LavaPipeline&) = delete;
    LavaPipeline& operator=(const LavaPipeline&) = delete;
    
    static void defaultPipelineConfigInfo(LavaPipelineConfigInfo& ConfigInfo);
    
    void Bind(VkCommandBuffer CommandBuffer);
    
private:
    
    static std::vector<char> readFile(const std::string& filePath);
    
    void createPipeline(const LavaPipelineConfigInfo& configInfo, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    
    void createShaderModule(const std::vector<char>& code, VkShaderModule* Module);
    
    // UNsafe
    LavaDevice& Device;
    VkPipeline Pipeline;
    VkShaderModule vertexShaderModule;
    VkShaderModule fragmentShaderModule;
};

}
