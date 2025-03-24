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
#include "LavaPipeline.hpp"
#include "LavaGameObject.hpp"
#include "LavaCamera.hpp"
#include "LavaTypes.hpp"

namespace lava {

class RenderSystem
{

#pragma region Lifecycle
    
public:
    
    RenderSystem(LavaDevice& InDevice, VkRenderPass InRenderPass, VkDescriptorSetLayout GlobalSetLayout);
    ~RenderSystem();
    
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
    
private:
    
    // #TODO remove this absolute path
    std::string absPathPrefix = "/Users/giorgiogamba/Documents/Projects/lava/";
    
    LavaDevice& Device;
    
#pragma endregion
    
#pragma region GameObjects
    
public:

    // Camera passed in argument in order to be shared between various systems
    void RenderGameObjects(const FrameDescriptor& FrameDesc);
    
#pragma endregion
    
#pragma region Pipeline
    
private:
    
    void CreatePipelineLayout(VkDescriptorSetLayout GlobalSetLayout);
    
    void CreatePipeline(VkRenderPass& RenderPass);
    
    std::unique_ptr<LavaPipeline> Pipeline;
    
    VkPipelineLayout PipelineLayout;
    
#pragma endregion
    
};

}

