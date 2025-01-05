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

// Vertex Buffer

// Passed as input to the graphics pipeline. It is a list of numbers that can be interpreted in different
// ways depending on the way we define descriptions. For example, we can define 2 position coordinates (layout 0) and 3 rgb values (layout 1) i in order to define a single vertex. The total vertex buffer will contain following series of 2 and 3 values. Another way to
// implement it is to create a single buffer for each kind of data we want to stored and bind each buffer to the pipeline. Anyway, in a buffer we have following vertices in following positions. A certain kind of data used to descrive a vertex is called "attribute"
// Each binding (which is the driver that connects the buffer to the pipeline) must be configured through a Vertex Binding Description, which is what instruction for the interpretation of the data contained inside the buffer. Each descriptor is composed by 3 elements:
// - binding index
// - input rate
// - stride (offset in bytes between two following vertices data)

// each attribute is defined by a descriptor:
// - binding
// - location
// - offset
// - format

// in order to save space, we are going to define a single big buffer and create different bindings pointing to different positions of the buffer depending on the kind of data needed

// Usually che single interleaved bnding in the best one but with the right algorithm

namespace Lve
{

#pragma region Types

// Interleaved implementation which alternates position and color inside the same buffer
struct Vertex
{
    glm::vec2 position;
    glm::vec3 color;
    
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
