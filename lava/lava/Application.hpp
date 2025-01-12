//
//  Application.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

// System includes
#include <stdio.h>
#include <string>
#include <memory>

// Local Includes
#include "LveWindow.hpp"
#include "LvePipeline.hpp"
#include "LveRenderer.hpp"
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

#pragma region Lifecycle
    
public:
    
    Application();
    ~Application();
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    void Run();
    
private:
    
    // #TODO remove this absolute path
    std::string absPathPrefix = "/Users/giorgiogamba/Documents/Projects/lava/lava/lava/";
    
    LveWindow Window{"TEST", WIDTH, HEIGTH};
    
    LveDevice Device{Window};
    
    LveRenderer Renderer{Window, Device};
    
#pragma endregion
    
#pragma region GameObjects
    
private:
    
    void LoadGameObjects();
    
    void RenderGameObjects(VkCommandBuffer CommandBuffer);
    
    std::vector<LveGameObject> GameObjects;
    
#pragma endregion
    
};

}
