//
//  Application.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include "Application.hpp"

#define GLM_FORCE_RADIANS // expects angles to be defined in radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "RenderSystem.hpp"

namespace Lve {

#pragma region Lifecycle

Application::Application()
{
    LoadGameObjects();
}

Application::~Application()
{
}

void Application::Run()
{
    RenderSystem RS{Device, Renderer.GetSwapChainRenderPass()};
    
    while (!Window.shouldClose())
    {
        glfwPollEvents();
        
        if (auto CommandBuffer = Renderer.StartDrawFrame())
        {
            // We kept this operations divided in order to add other drawing elements in between
            
            Renderer.StartSwapChainRenderPass(CommandBuffer);
            RS.RenderGameObjects(CommandBuffer, GameObjects);
            Renderer.EndSwapChainRenderPass(CommandBuffer);
            Renderer.EndDrawFrame();
        }
    }
    
    // Makes the CPU wait until all the GPU resources are freed
    vkDeviceWaitIdle(Device.device());
}

#pragma endregion

#pragma region GameObjects

void Application::LoadGameObjects()
{
    // Initializes position and color for each vertex
    std::vector<Vertex> Vertices =
    { {{0.f, -0.5f}, {1.f, 0.f, 0.f}}
    , {{0.5f, 0.5f}, {0.f, 1.f, 0.f}}
    , {{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}};
    
    // Just define a single model that is shared between multiple game objects
    const std::shared_ptr<LveModel> Model = std::make_shared<LveModel>(Device, Vertices);
    
    LveGameObject Triangle = LveGameObject::CreateGameObject();
    Triangle.SetModel(Model);
    Triangle.SetColor({.1f, .8f, .1f});
    
    Triangle.Transform2D.Translation.x = .2f;
    Triangle.Transform2D.Scale = {2.f, .5f};
    Triangle.Transform2D.RotationAngle = .25f * glm::two_pi<float>();
    
    GameObjects.push_back(std::move(Triangle));
}

#pragma endregion

}
