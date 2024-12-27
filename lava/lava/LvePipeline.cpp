//
//  LvePipeline.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include <fstream>
#include <iostream>

#include "LvePipeline.hpp"

namespace Lve {

LvePipeline::LvePipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    createPipeline(vertexShaderPath, fragmentShaderPath);
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

void LvePipeline::createPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    const std::vector<char>& vertexShader = readFile(vertexShaderPath);
    const std::vector<char>& fragmentShader = readFile(fragmentShaderPath);
    
    std::cout << "Shaders allocation completed" << "\n";
    std::cout << "Vertex shader size: " << vertexShader.size() << "\n";
    std::cout << "Fragment shader size: " << fragmentShader.size() << std::endl;
}

}
