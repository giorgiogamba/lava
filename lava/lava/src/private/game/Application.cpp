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
#include "LveCamera.hpp"

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
    LveCamera Camera{};
    
    Camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
    Camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
    
    while (!Window.shouldClose())
    {
        glfwPollEvents();
        
        // We compute projection at every frame so that the view volume aspect ratio is always updated with the window
        const float aspectRatio = Renderer.GetAspectRatio();
        //Camera.SetOrthoProjection(-aspectRatio, aspectRatio, -1.f, 1.f, -1.f, 1.f);
        Camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 10.f);
        
        if (auto CommandBuffer = Renderer.StartDrawFrame())
        {
            // We kept this operations divided in order to add other drawing elements in between
            
            Renderer.StartSwapChainRenderPass(CommandBuffer);
            RS.RenderGameObjects(CommandBuffer, GameObjects, Camera);
            Renderer.EndSwapChainRenderPass(CommandBuffer);
            Renderer.EndDrawFrame();
        }
    }
    
    // Makes the CPU wait until all the GPU resources are freed
    vkDeviceWaitIdle(Device.device());
}

#pragma endregion

#pragma region GameObjects

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<LveModel> Application::createCubeModel(LveDevice& device, glm::vec3 offset) {
  std::vector<Vertex> vertices{

      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<LveModel>(device, vertices);
}

void Application::LoadGameObjects()
{
    const std::shared_ptr<LveModel> Model = createCubeModel(Device, {.0f, .0f, .0f});
    LveGameObject Cube = LveGameObject::CreateGameObject();
    Cube.SetModel(Model);
    
    // Vulkan Canonical View Volume: X(-1, 1) Y(-1, 1) Z(0, 1)
    // where +X is to the right, +Y is to the botton and +Z is in the opposite way respect to the user
    // We translate the cube to the middle of the canonical view volume so that it is not cut by scissors when rotating
    Cube.Transform.Translation = {.0f, .0f, 2.5f};
    Cube.Transform.Scale = {.5f, .5f, .5f};
    
    GameObjects.push_back(std::move(Cube));
        
}

#pragma endregion

}
