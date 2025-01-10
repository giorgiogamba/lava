//
//  Application.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <string>
#include <memory>

#endif /* Application_hpp */

#include "LveWindow.hpp"
#include "LvePipeline.hpp"
#include "LveSwapChain.hpp"
#include "LveGameObject.hpp"

namespace Lve {

#pragma region Constants

static constexpr int WIDTH = 800;
static constexpr int HEIGTH = 800;

#pragma endregion

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

class Application
{
public:
    
    Application();
    ~Application();
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    void Run();
    
private:
    
    void LoadGameObjects();
    void CreatePipelineLayout();
    void CreatePipeline();
    void CreateCommandBuffers();
    
    void freeCommandBuffers();
    
    void DrawFrame();
    
    void RecreateSwapChain();
    void RecordCommandBuffer(const int ImgIdx);
    
    // #TODO remove this absolute path
    std::string absPathPrefix = "/Users/giorgiogamba/Documents/Projects/lava/lava/lava/";
    
    LveWindow Window{"TEST", WIDTH, HEIGTH};
    
    LveDevice Device{Window};
    
    std::unique_ptr<LveSwapChain> SwapChain;
    
    std::unique_ptr<LvePipeline> Pipeline;
    VkPipelineLayout PipelineLayout;
    std::vector<VkCommandBuffer> CommandBuffers;
    
    std::vector<LveGameObject> GameObjects;
};

}
