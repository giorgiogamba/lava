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
#include "LveCamera.hpp"

namespace Lve {

class RenderSystem
{

#pragma region Lifecycle
    
public:
    
    RenderSystem(LveDevice& InDevice, VkRenderPass InRenderPass);
    ~RenderSystem();
    
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
    
private:
    
    // #TODO remove this absolute path
    std::string absPathPrefix = "/Users/giorgiogamba/Documents/Projects/lava/";
    
    LveDevice& Device;
    
#pragma endregion
    
#pragma region GameObjects
    
public:

    // Camera passed in argument in order to be shared between various systems
    void RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<LveGameObject>& GameObjects, const LveCamera& Camera);
    
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

