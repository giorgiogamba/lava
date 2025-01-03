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

#endif /* LvePipeline_hpp */

namespace Lve {

#pragma region Types

struct LvePipelineConfigInfo
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
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
    void operator=(const LvePipeline&) = delete;
    
    static LvePipelineConfigInfo defaultPipelineConfigInfo(const uint32_t Width, const uint32_t Height);
    
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
