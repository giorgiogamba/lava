//
//  LvePipeline.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

#ifndef LvePipeline_hpp
#define LvePipeline_hpp

#include <stdio.h>
#include <string>
#include <vector>

#include "LveDevice.hpp"

#include <vulkan/vulkan.h>

#endif /* LvePipeline_hpp */

namespace Lve {

#pragma region Types

struct LvePipelineConfigInfo
{
    LvePipelineConfigInfo() = default;
    
    LvePipelineConfigInfo(const LvePipelineConfigInfo&) = delete;
    LvePipelineConfigInfo& operator=(LvePipelineConfigInfo&) = delete;
    
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

class LvePipeline
{
public:
    
    LvePipeline(LveDevice& InDevice, const LvePipelineConfigInfo& configInfo, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~LvePipeline();
    
    LvePipeline(const LvePipeline&) = delete;
    LvePipeline& operator=(const LvePipeline&) = delete;
    
    static void defaultPipelineConfigInfo(LvePipelineConfigInfo& ConfigInfo);
    
    void Bind(VkCommandBuffer CommandBuffer);
    
private:
    
    static std::vector<char> readFile(const std::string& filePath);
    
    void createPipeline(const LvePipelineConfigInfo& configInfo, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    
    void createShaderModule(const std::vector<char>& code, VkShaderModule* Module);
    
    // UNsafe
    LveDevice& Device;
    VkPipeline Pipeline;
    VkShaderModule vertexShaderModule;
    VkShaderModule fragmentShaderModule;
};

}
