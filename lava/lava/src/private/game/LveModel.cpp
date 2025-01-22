//
//  LveModel.cpp
//  lava
//
//  Created by Giorgio Gamba on 03/01/25.
//

#include "LveModel.hpp"

#include <assert.h>

namespace Lve
{

#pragma region Types

std::vector<VkVertexInputBindingDescription> Vertex::GetBindingDesc()
{
    // Binding description for a single vertex buffer
    std::vector<VkVertexInputBindingDescription> BindingDescs(1);
    BindingDescs[0].binding = 0;
    BindingDescs[0].stride = sizeof(Vertex);
    BindingDescs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    return BindingDescs;
}

std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescs()
{
    std::vector<VkVertexInputAttributeDescription> AttributeDescs(2);
    AttributeDescs[0].binding = 0;
    AttributeDescs[0].location = 0;                         // Matches shaders
    AttributeDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT;  // Also if a position, we use color coordinates
    AttributeDescs[0].offset = 0;  // 0
    
    AttributeDescs[1].binding = 0;
    AttributeDescs[1].location = 1;                          // Matches shaders
    AttributeDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    AttributeDescs[1].offset = offsetof(Vertex, color);
    
    return AttributeDescs;
}

#pragma endregion

LveModel::LveModel(LveDevice& InDevice, const Builder& Builder)
    : Device(InDevice)
    , bHasIndexBuffer(false)
{
    CreateVertexBuffers(Builder.Vertices);
    CreateIndexBuffers(Builder.Indices);
}

LveModel::~LveModel()
{
    // NOTE: When creating complex scenes, we will run into allocation problems, thus it will be convenient to add
    // a memory allocator like the one defined in
    // https://www.youtube.com/redirect?event=video_description&redir_token=QUFFLUhqbU5VUnJqS2c1YWJJTE5oV1c0TjNCSUFyUEtud3xBQ3Jtc0ttZzBQWXJsZWJZcVZzRUlSeDFzV0Y1NkhVam1NdVZQUXJnclllcWY1VVI0eklDcFFGbUFhWGFzaFJDSTlDQkFZZ0F0d0RqMmVjdkF2enFJTFFtRTY2U1FPLS1aeUEwUWZSQ25rcEpfeTAta0xMZi1CNA&q=http%3A%2F%2Fkylehalladay.com%2Fblog%2Ftutorial%2F2017%2F12%2F13%2FCustom-Allocators-Vulkan.html&v=mnKp501RXDc
    vkDestroyBuffer(Device.device(), VertexBuffer, nullptr);
    vkFreeMemory(Device.device(), VertexBufferMemory, nullptr);
    
    if (bHasIndexBuffer)
    {
        vkDestroyBuffer(Device.device(), IndexBuffer, nullptr);
        vkFreeMemory(Device.device(), IndexBufferMemory, nullptr);
    }
}

void LveModel::Bind(VkCommandBuffer& CommandBuffer)
{
    VkBuffer Buffers[] = {VertexBuffer};
    VkDeviceSize Offsets[] = {0};
    vkCmdBindVertexBuffers(CommandBuffer, 0, 1, Buffers, Offsets);
    
    if (bHasIndexBuffer)
    {
        // 32bit index type for huge storing
        vkCmdBindIndexBuffer(CommandBuffer, IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
}

void LveModel::Draw(VkCommandBuffer& CommandBuffer)
{
    if (bHasIndexBuffer)
    {
        // Index drawing
        vkCmdDrawIndexed(CommandBuffer, IndexCount, 1, 0, 0, 0);
    }
    else
    {
        // Vertex drawing
        vkCmdDraw(CommandBuffer, VertexCount, 1, 0, 0);
    }
}

#pragma region Vertices

void LveModel::CreateVertexBuffers(const std::vector<Vertex>& Vertices)
{
    VertexCount = static_cast<uint32_t>(Vertices.size());
    // We check to have at least 3 elements, meaning that the model represents a triangle
    assert(VertexCount >= 3 && "NOTE: Vertex count must be at least 3");
    
    // Compute the total number of bytes required to store the current model
    const VkDeviceSize BufferSize = sizeof(Vertices[0]) * VertexCount;
    
    // Visible makes the memory visible to the CPU, Coherent keeps the host and device memory regions consistent to each others
    Device.createBuffer(BufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexBuffer, VertexBufferMemory);
    
    void* data;
    vkMapMemory(Device.device(), VertexBufferMemory, 0, BufferSize, 0, &data);
    
    // copies vertices data to the host map memory region. Since we set the Coherent bit to true, then since
    // the host memory is updated, then the device memory is updated consequently
    memcpy(data, Vertices.data(), static_cast<size_t>(BufferSize));
    
    vkUnmapMemory(Device.device(), VertexBufferMemory);
}

#pragma endregion

#pragma region Indices

void LveModel::CreateIndexBuffers(const std::vector<uint32_t>& Indices)
{
    IndexCount = static_cast<uint32_t>(Indices.size());
    
    bHasIndexBuffer = IndexCount > 0;
    if (!bHasIndexBuffer)
        return;
    
    // Compute the total number of bytes required to store the current model
    const VkDeviceSize BufferSize = sizeof(Indices[0]) * IndexCount;
    
    // Visible makes the memory visible to the GPU, Coherent keeps the host and device memory regions consistent to each others
    Device.createBuffer(BufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexBuffer, IndexBufferMemory);
    
    // Creates a reference on the CPU of the data allocated in the GPU.
    // When it is updated (when someone writes in it), it creates a copy on the GPU
    void* data;
    vkMapMemory(Device.device(), IndexBufferMemory, 0, BufferSize, 0, &data);
    
    // Copies vertices data to the host map memory region. Since we set the Coherent bit to true, then since
    // the host memory is updated, then the device memory is updated consequently
    memcpy(data, Indices.data(), static_cast<size_t>(BufferSize));
    
    // Since we don't need data information on the host (CPU), we clear it so that we have it only on GPU
    vkUnmapMemory(Device.device(), IndexBufferMemory);
}

#pragma endregion

}
