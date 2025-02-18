//
//  LavaModel.cpp
//  lava
//
//  Created by Giorgio Gamba on 03/01/25.
//

#include "LavaModel.hpp"

#include <assert.h>
#include <iostream>
#include <vector>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyObjLoader.h"

#include "LavaUtils.hpp"

// Enable has functionality for glm::vec3
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std
{
    template<>
    struct hash<lava::Vertex>
    {
        // Used to insert Vertex elements inside a map
        size_t operator()(const lava::Vertex& Vertex) const
        {
            size_t Seed = 0;
            lava::HashCombine(Seed, Vertex.position, Vertex.color, Vertex.normal, Vertex.uv);
            return Seed;
        }
    };
}

namespace lava
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
    std::vector<VkVertexInputAttributeDescription> AttributeDescs{};
    AttributeDescs.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
    AttributeDescs.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
    AttributeDescs.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
    AttributeDescs.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)}); //uv are 2D
    
    return AttributeDescs;
}

void Builder::LoadModel(const std::string& Filename)
{
    tinyobj::attrib_t Attrib;
    std::vector<tinyobj::shape_t> Shapes;
    std::vector<tinyobj::material_t> Materials;
    std::string Warning;
    std::string Error;

    if (!tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warning, &Error, Filename.c_str()))
    {
        throw std::runtime_error(Warning + Error);
    }

    // Object loading succeded
    Vertices.clear();
    Indices.clear();

    std::unordered_map<Vertex, uint32_t> UniqueVertices;

    for (const auto& Shape : Shapes)
    {
        for (const auto& Index : Shape.mesh.indices)
        {
            Vertex V{};
            
            // Suppose wa have faces defined as vertexIdx/normalIdx/uvIdx

            if (Index.vertex_index >= 0)
            {
                // Suppose each vertex is a vec3, then each component is in following positions
                // with offset 0, 1 and 2 starting from the vertex index
                const size_t startingIdx = 3 * Index.vertex_index;
                V.position =
                {
                    Attrib.vertices[startingIdx],
                    Attrib.vertices[startingIdx + 1], 
                    Attrib.vertices[startingIdx + 2]
                };        

                // Supposed the provided file owns colors after vertices positions
                V.color =
                {
                    Attrib.colors[startingIdx],
                    Attrib.colors[startingIdx + 1], 
                    Attrib.colors[startingIdx + 2]
                }; 
            }

            if (Index.normal_index >= 0)
            {
                const size_t startingIdx = 3 * Index.normal_index;
                V.normal =
                {
                    Attrib.normals[startingIdx],
                    Attrib.normals[startingIdx + 1], 
                    Attrib.normals[startingIdx + 2]
                };                
            }

            if (Index.texcoord_index >= 0)
            {
                const size_t startingIdx = 2 * Index.texcoord_index;
                V.uv =
                {
                    Attrib.texcoords[startingIdx],
                    Attrib.texcoords[startingIdx + 1]
                };    
            }

            if (UniqueVertices.count(V) == 0)
            {
                // If the Vertex V is not contained inside the map. We keep track of its position inside the Vertices vector
                // by keeping track of how many vertices there currently are inside the vector
                UniqueVertices[V] = static_cast<uint32_t>(Vertices.size());
                Vertices.push_back(V);
            }

            // The index of the vertex is its position inside the Vertices array, stored in the UniqueVertices map
            Indices.push_back(UniqueVertices[V]);
        }
    }
}

#pragma endregion

LavaModel::LavaModel(LavaDevice& InDevice, const Builder& Builder)
    : Device(InDevice)
    , bHasIndexBuffer(false)
{
    CreateVertexBuffers(Builder.Vertices);
    CreateIndexBuffers(Builder.Indices);
}

LavaModel::~LavaModel() {}

std::unique_ptr<LavaModel> LavaModel::CreateModelFromFile(LavaDevice& Device, const std::string& Filepath)
{
    Builder ModelBuilder{};
    ModelBuilder.LoadModel(Filepath);

    std::cout << "Vertex count: " << ModelBuilder.Vertices.size() << std::endl;

    return std::make_unique<LavaModel>(Device, ModelBuilder);
}

void LavaModel::Bind(VkCommandBuffer& CommandBuffer)
{
    VkBuffer Buffers[] = {VertexBuffer->getBuffer()};
    VkDeviceSize Offsets[] = {0};
    vkCmdBindVertexBuffers(CommandBuffer, 0, 1, Buffers, Offsets);
    
    if (bHasIndexBuffer)
    {
        // 32bit index type for huge storing
        vkCmdBindIndexBuffer(CommandBuffer, IndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}

void LavaModel::Draw(VkCommandBuffer& CommandBuffer)
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

void LavaModel::ClearBufferAndMemory(VkBuffer& Buffer, VkDeviceMemory& Memory)
{
    vkDestroyBuffer(Device.device(), Buffer, nullptr);
    vkFreeMemory(Device.device(), Memory, nullptr);
}

#pragma region Vertices

void LavaModel::CreateVertexBuffers(const std::vector<Vertex>& Vertices)
{
    VertexCount = static_cast<uint32_t>(Vertices.size());
    // We check to have at least 3 elements, meaning that the model represents a triangle
    assert(VertexCount >= 3 && "NOTE: Vertex count must be at least 3");
    
    // Compute the total number of bytes required to store the current model
    const VkDeviceSize BufferSize = sizeof(Vertices[0]) * VertexCount;

    const uint32_t VertexSize = sizeof(Vertices[0]);

    // Copies data from CPU to GPU
    LavaBuffer StagingBuffer
        { Device
        , VertexSize
        , VertexCount
        , VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

    StagingBuffer.map();
    StagingBuffer.writeToBuffer((void*) Vertices.data());

    VertexBuffer = std::make_unique<LavaBuffer>
        ( Device
        , VertexSize
        , VertexCount
        , VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
    
    Device.copyBuffer(StagingBuffer.getBuffer(), VertexBuffer->getBuffer(), BufferSize);
}

#pragma endregion

#pragma region Indices

void LavaModel::CreateIndexBuffers(const std::vector<uint32_t>& Indices)
{
    IndexCount = static_cast<uint32_t>(Indices.size());
    
    bHasIndexBuffer = IndexCount > 0;
    if (!bHasIndexBuffer)
        return;
    
    VkDeviceSize BufferSize = sizeof(Indices[0]) * IndexCount;

    const uint64_t IndexSize = sizeof(Indices[0]);

    LavaBuffer StagingBuffer
        { Device
        , IndexSize
        , IndexCount
        , VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
    
    StagingBuffer.map();
    StagingBuffer.writeToBuffer((void*) Indices.data());

    IndexBuffer = std::make_unique<LavaBuffer>
        ( Device
        , IndexSize
        , IndexCount
        , VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    Device.copyBuffer(StagingBuffer.getBuffer(), IndexBuffer->getBuffer(), BufferSize);
}

#pragma endregion

}
