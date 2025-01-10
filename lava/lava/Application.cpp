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

namespace Lve {

Application::Application()
{
    LoadGameObjects();
    CreatePipelineLayout();
    RecreateSwapChain();
    CreateCommandBuffers();
}

Application::~Application()
{
    vkDestroyPipelineLayout(Device.device(), PipelineLayout, nullptr);
}

void Application::Run()
{
    while (!Window.shouldClose())
    {
        glfwPollEvents();
        
        DrawFrame();
    }
    
    // Makes the CPU wait until all the GPU resources are freed
    vkDeviceWaitIdle(Device.device());
}

void Application::CreatePipelineLayout()
{ 
    VkPushConstantRange PushConstantRange{};
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(PushConstantRange);
    
    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pSetLayouts = nullptr;
    PipelineLayoutInfo.pushConstantRangeCount = 1;
    PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;
    
    if (vkCreatePipelineLayout(Device.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void Application::CreatePipeline()
{
    assert(SwapChain && "Swap Chain is null");
    assert(PipelineLayout && "Pipeline Layout is null");
    
    LvePipelineConfigInfo PipelineConfigInfo;
    LvePipeline::defaultPipelineConfigInfo(PipelineConfigInfo);
    
    // Basically the render pass says to the graphics pipeline what kind of output to create
    // (meaning how color buffer, depth etc. are allocated in the frame buffer)
    PipelineConfigInfo.renderPass = SwapChain->getRenderPass();
    PipelineConfigInfo.pipelineLayout = PipelineLayout;
    Pipeline = std::make_unique<LvePipeline>(Device, PipelineConfigInfo, absPathPrefix+"shaders/vertex_shader.vert.spv", absPathPrefix+"shaders/fragment_shader.frag.spv");
}

void Application::CreateCommandBuffers()
{
    // Create a number of buffers equal to the number of drawable frames
    CommandBuffers.resize(SwapChain->imageCount());
    
    VkCommandBufferAllocateInfo AllocationInfo{};
    AllocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // This means the buffer can be submitted to a queue for execution
    // Secondary command buffers cannot be submitted to queues but can be called by other command buffers
    AllocationInfo.commandPool = Device.getCommandPool();
    AllocationInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());
    
    if (vkAllocateCommandBuffers(Device.device(), &AllocationInfo, CommandBuffers.data()) != VK_SUCCESS)
    {
        throw new std::runtime_error("Failed whiel creating command buffer");
    }
}

void Application::RecreateSwapChain()
{
    auto Extent = Window.getExtent();
    while (Extent.width == 0 || Extent.height == 0)
    {
        Extent = Window.getExtent();
        glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(Device.device());
    
    if (!SwapChain)
    {
        SwapChain = std::make_unique<LveSwapChain>(Device, Extent);
    }
    else
    {
        SwapChain = std::make_unique<LveSwapChain>(Device, Extent, std::move(SwapChain));
        
        if (SwapChain->imageCount() != CommandBuffers.size())
        {
            freeCommandBuffers();
            CreateCommandBuffers();
        }
    }
    
    CreatePipeline();
}

void Application::freeCommandBuffers()
{
  vkFreeCommandBuffers(Device.device(), Device.getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
  CommandBuffers.clear();
}

void Application::RecordCommandBuffer(const int ImgIdx)
{
    VkCommandBufferBeginInfo BeginInfo{};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(CommandBuffers[ImgIdx], &BeginInfo) != VK_SUCCESS)
    {
        throw new std::runtime_error("Failed to begin recording command buffer");
    }
    
    VkRenderPassBeginInfo RenderPassBeginInfo{};
    RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    RenderPassBeginInfo.renderPass = SwapChain->getRenderPass();
    RenderPassBeginInfo.framebuffer = SwapChain->getFrameBuffer(ImgIdx);
    
    RenderPassBeginInfo.renderArea.offset = {0, 0};
    RenderPassBeginInfo.renderArea.extent = SwapChain->getSwapChainExtent(); // solves resolution problems
    
    // This represents a clear representation of the frame buffers
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.f, 0.f, 0.f, 1.f};
    // We avoid clearValues[0].depthStencil because we organized depthbuffers so that in 0 we have color buffer and in 1 depthbuffer
    clearValues[1].depthStencil =  {1.f, 0};
    RenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    RenderPassBeginInfo.pClearValues = clearValues.data();
    
    // Stores command buffer in primary command buffer
    // We cannot have a render pass that uses both primary and secondary buffers
    vkCmdBeginRenderPass(CommandBuffers[ImgIdx], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkViewport Viewport{};
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = static_cast<float>(SwapChain->getSwapChainExtent().width);
    Viewport.height = static_cast<float>(SwapChain->getSwapChainExtent().height);
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;
    vkCmdSetViewport(CommandBuffers[ImgIdx], 0, 1, &Viewport);
    
    VkRect2D Scissor{{0, 0}, SwapChain->getSwapChainExtent()};
    vkCmdSetScissor(CommandBuffers[ImgIdx], 0, 1, &Scissor);
    
    RenderGameObjects(CommandBuffers[ImgIdx]);
    
    vkCmdEndRenderPass(CommandBuffers[ImgIdx]);
    if (vkEndCommandBuffer(CommandBuffers[ImgIdx]) !=  VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end command buffer");
    }
}

void Application::RenderGameObjects(VkCommandBuffer CommandBuffer)
{
    Pipeline->Bind(CommandBuffer);
    
    for (LveGameObject& GameObject : GameObjects)
    {
        PushConstantData PushConstant{};
        PushConstant.offset = GameObject.Transform2D.Translation;
        PushConstant.color = GameObject.GetColor();
        PushConstant.transform = GameObject.Transform2D.mat2();
        
        vkCmdPushConstants(CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &PushConstant);
        
        GameObject.GetModel()->Bind(CommandBuffer);
        GameObject.GetModel()->Draw(CommandBuffer);
    }
}

void Application::DrawFrame()
{
    // Retrieve the information about the frame to draw next
    uint32_t ImageIdx;
    VkResult Result = SwapChain->acquireNextImage(&ImageIdx);
    
    // Checks if the swap chain is still valid after resizing and before drawing
    if (Result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }
    
    if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire next swap chain image");
    }
    
    RecordCommandBuffer(ImageIdx);
    
    // Provides commands to the device graphics queue (makes also sync)
    Result = SwapChain->submitCommandBuffers(&CommandBuffers[ImageIdx], &ImageIdx);
    
    if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || Window.WasWindowResized())
    {
        Window.ResetWindowResizedStatus();
        RecreateSwapChain();
    }
}

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
    
    GameObjects.push_back(std::move(Triangle));
}

}
