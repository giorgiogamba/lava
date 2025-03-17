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

#include <chrono>
#include <iostream>
#include <filesystem>

#include "RenderSystem.hpp"
#include "LavaCamera.hpp"
#include "KeyboardMovementController.hpp"

namespace lava {

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
    std::vector<std::unique_ptr<LavaBuffer>> UBOBuffers(LavaSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < UBOBuffers.size(); ++i)
    {
        UBOBuffers[i] = std::make_unique<LavaBuffer>
            ( Device
            , sizeof(UniformBuffer)
            , 1 // In case you have multiple copies for instance
            , VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
            , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

        UBOBuffers[i]->map();
    }

    auto GlobalSetLayout = LavaDescriptorSetLayout::Builder(Device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .build();

    std::vector<VkDescriptorSet> GlobalDescriptorSets(LavaSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < GlobalDescriptorSets.size(); ++i)
    {
        auto BufferInfo = UBOBuffers[i]->descriptorInfo();
        LavaDescriptorWriter(*GlobalSetLayout, *GlobalPool)
            .writeBuffer(0,  &BufferInfo)
            .build(GlobalDescriptorSets[i]);
    }

    RenderSystem RS{Device, Renderer.GetSwapChainRenderPass(), GlobalSetLayout->getDescriptorSetLayout()};
    LavaCamera Camera{};
    
    Camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
    Camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
    
    // Saves camera sate
    LavaGameObject ViewerObject = LavaGameObject::CreateGameObject();
    
    KeyboardMovementController CameraController{};
    
    auto CurrentTime = std::chrono::high_resolution_clock::now();
    
    while (!Window.shouldClose())
    {
        // Blocking action
        glfwPollEvents();
        
        auto NewTime = std::chrono::high_resolution_clock::now();
        
        // 1 means 1 second, 0.5 means half-second
        const float DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(NewTime - CurrentTime).count();
        CurrentTime = NewTime;
        
        // Eventually limit the DeltaTime to a max value in order to work while resizing the window
        
        CameraController.MoveInPlaneXZ(Window.GetGLFWwindow(), DeltaTime, ViewerObject);
        Camera.SetViewYX(ViewerObject.Transform.Translation, ViewerObject.Transform.Rotation);
        
        // We compute projection at every frame so that the view volume aspect ratio is always updated with the window
        const float aspectRatio = Renderer.GetAspectRatio();
        //Camera.SetOrthoProjection(-aspectRatio, aspectRatio, -1.f, 1.f, -1.f, 1.f);
        Camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 10.f);
        
        if (auto CommandBuffer = Renderer.StartDrawFrame())
        {
            const int FrameIdx = Renderer.GetFrameIdx();

            FrameDescriptor FrameDesc{FrameIdx, DeltaTime, CommandBuffer, Camera, GlobalDescriptorSets[FrameIdx]};

            // Update objects and memory
            UniformBuffer UBO{};
            UBO.ProjectionMatrix = Camera.GetProjectionMat() * Camera.GetViewMat();
            UBOBuffers[FrameIdx]->writeToBuffer(&UBO);
            UBOBuffers[FrameIdx]->flush();

            // Drawing
            Renderer.StartSwapChainRenderPass(CommandBuffer);
            RS.RenderGameObjects(FrameDesc, GameObjects);
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
    const std::filesystem::path modelPath = std::filesystem::absolute("models/smooth_vase.obj");
    const std::shared_ptr<LavaModel> Model = LavaModel::CreateModelFromFile(Device, modelPath);
    LavaGameObject GameObject = LavaGameObject::CreateGameObject();
    GameObject.SetModel(Model);
    
    // Vulkan Canonical View Volume: X(-1, 1) Y(-1, 1) Z(0, 1)
    // where +X is to the right, +Y is to the botton and +Z is in the opposite way respect to the user
    // We translate the cube to the middle of the canonical view volume so that it is not cut by scissors when rotating
    GameObject.Transform.Translation = {.0f, .0f, 2.5f};
    GameObject.Transform.Scale = {.5f, .5f, .5f};
    
    GameObjects.push_back(std::move(GameObject));
}

#pragma endregion

}
