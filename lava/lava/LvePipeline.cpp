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
    LvePipelineConfigInfo ConfigInfo;
    
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
    const std::vector<char>& vertexShader = readFile(vertexShaderPath);
    const std::vector<char>& fragmentShader = readFile(fragmentShaderPath);
    
    std::cout << "Shaders allocation completed" << "\n";
    std::cout << "Vertex shader size: " << vertexShader.size() << "\n";
    std::cout << "Fragment shader size: " << fragmentShader.size() << std::endl;
}

void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* Module)
{
    VkShaderModuleCreateInfo createInfo;
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
