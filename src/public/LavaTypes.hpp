//
//  LavaTypes.hpp
//  lava
//
//  Created by Giorgio Gamba on 13/01/25.
//

#pragma once

#define GLM_FORCE_RADIANS // expects angles to be defined in radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#include "LavaCamera.hpp"

#pragma region PushConstants

struct PushConstant2DData
{
    glm::mat2 transform{1.f};
    
    glm::vec2 offset;
    
    // We align the field becuase of the requirement made by Push Constants, which need to define
    // multiples of N in size 2 (N, 2N, 4N, ...). In case of colors, they must be in 4N, which means
    // that a padding between offset and color will be set in order to make the elements aligned
    alignas(16) glm::vec3 color;
};

struct PushConstant3DData
{
    glm::mat4 ModelMatrix{1.f};
    
    // We align the field becuase of the requirement made by Push Constants, which need to define
    // multiples of N in size 2 (N, 2N, 4N, ...). In case of colors, they must be in 4N, which means
    // that a padding between offset and color will be set in order to make the elements aligned
    //alignas(16) glm::vec3 color;

    glm::mat4 normalMatrix{1.f};
};

#pragma endregion

#pragma region Uniform Buffers

struct UniformBuffer
{
    glm::mat4 ProjectionMatrix{1.f};
    
    glm::vec4 AmbientLightCol{1.f, 1.f, 1.f, 0.2f}; // w is intensity

    // Since vec3 and vec4 need to be aligned respect to 16 bytes, 
    // we need to add a padding or use aligned as

    // Point Light Definition
    glm::vec3 PointLightPos{-1.f};
    alignas(16) glm::vec4 PointLightCol{1.f}; // w coord contains light intensity
};

#pragma endregion

#pragma region Frame

namespace lava
{

struct FrameDescriptor
{
    int FrameIdx;
    float FrameTimeInS;
    VkCommandBuffer CommandBuffer;
    LavaCamera& Camera;
    VkDescriptorSet GlobalDescriptorSet;
};

}

#pragma endregion
