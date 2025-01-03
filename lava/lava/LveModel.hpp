//
//  LveModel.hpp
//  lava
//
//  Created by Giorgio Gamba on 03/01/25.
//

#pragma once

#ifndef LveModel_hpp
#define LveModel_hpp

#include <stdio.h>
#include <vector>

#define GLM_FORCE_RADIANS // expects angles to be defined in radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "LveDevice.hpp"

#endif /* LveModel_hpp */

namespace Lve
{

#pragma region Types

struct Vertex
{
    glm::vec2 position;
    static std::vector<VkVertexInputBindingDescription> GetBindingDesc();
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescs();
};
#pragma endregion

class LveModel
{
public:
    
    LveModel(LveDevice& InDevice, const std::vector<Vertex>& InVertices);
    ~LveModel();
    
    LveModel(const LveModel&) = delete;
    LveModel& operator=(const LveModel&) = delete;
    
    void Bind(VkCommandBuffer& CommandBuffer);
    void Draw(VkCommandBuffer& CommandBuffer);
    
private:
    
    void CreateVertexBuffers(const std::vector<Vertex>& Vertices);
    
    LveDevice& Device;
    
    VkBuffer VertexBuffer;
    VkDeviceMemory VertexBufferMemory;
    uint32_t VertexCount;
    
};

}
