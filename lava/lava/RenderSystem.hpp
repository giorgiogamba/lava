//
//  RenderSystem.hpp
//  lava
//
//  Created by Giorgio Gamba on 10/01/25.
//

/**
 
 With "Render System", we means all the methods that permit to render a certain set of objects in a certain manner
 The overall project structure, in the end, will contain a single Renderer, which handles the main application elements,
 and a series of RenderSystems that handles subsets of objects to render in a certain way (e.g. material system)
 
 */

#pragma once

// System includes
#include <stdio.h>
#include <string>
#include <memory>

// Local Includes
#include "LvePipeline.hpp"
#include "LveGameObject.hpp"

namespace Lve {

class RenderSystem
{
    
#pragma region Types

struct PushConstantData
{
    glm::mat2 transform{1.f};
    
    glm::vec2 offset;
    
    // We align the field becuase of the requirement made by Push Constants, which need to define
    // multiples of N in size 2 (N, 2N, 4N, ...). In case of colors, they must be in 4N, which means
    // that a padding between offset and color will be set in order to make the elements aligned
    alignas(16) glm::vec3 color;
};

#pragma endregion

#pragma region Lifecycle
    
public:
    
    RenderSystem(LveDevice& InDevice, VkRenderPass InRenderPass);
    ~RenderSystem();
    
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
    
private:
    
    // #TODO remove this absolute path
    std::string absPathPrefix = "/Users/giorgiogamba/Documents/Projects/lava/lava/lava/";
    
    LveDevice& Device;
    
#pragma endregion
    
#pragma region GameObjects
    
public:

    void RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<LveGameObject>& GameObjects);
    
#pragma endregion
    
#pragma region Pipeline
    
private:
    
    void CreatePipelineLayout();
    
    void CreatePipeline(VkRenderPass& RenderPass);
    
    std::unique_ptr<LvePipeline> Pipeline;
    
    VkPipelineLayout PipelineLayout;
    
#pragma endregion
    
};

}

