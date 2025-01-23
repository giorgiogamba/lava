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
#include "LavaWindow.hpp"
#include "LavaPipeline.hpp"
#include "LavaRenderer.hpp"
#include "LavaGameObject.hpp"

namespace Lava {

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
    
    std::unique_ptr<LavaModel> createCubeModel(LavaDevice& device, glm::vec3 offset);
    
    LavaWindow Window{"TEST", WIDTH, HEIGTH};
    
    LavaDevice Device{Window};
    
    LavaRenderer Renderer{Window, Device};
    
#pragma endregion
    
#pragma region GameObjects
    
private:
    
    void LoadGameObjects();
    
    std::vector<LavaGameObject> GameObjects;
    
#pragma endregion
    
};

}
