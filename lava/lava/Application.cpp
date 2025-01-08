//
//  Application.cpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#include "Application.hpp"

namespace Lve {

Application::Application()
{
    LoadModels();
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
    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pSetLayouts = nullptr;
    PipelineLayoutInfo.pushConstantRangeCount = 0;
    PipelineLayoutInfo.pPushConstantRanges = nullptr;
    
    if (vkCreatePipelineLayout(Device.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void Application::CreatePipeline()
{
    LvePipelineConfigInfo PipelineConfigInfo = LvePipeline::defaultPipelineConfigInfo(SwapChain->width(), SwapChain->height());
    
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
    SwapChain = std::make_unique<LveSwapChain>(Device, Extent);
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
    clearValues[1].depthStencil =  {static_cast<uint32_t>(1.f), static_cast<uint32_t>(0.f)};
    RenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    RenderPassBeginInfo.pClearValues = clearValues.data();
    
    // Stores command buffer in primary command buffer
    // We cannot have a render pass that uses both primary and secondary buffers
    vkCmdBeginRenderPass(CommandBuffers[ImgIdx], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    Pipeline->Bind(CommandBuffers[ImgIdx]);
    
    Model->Bind(CommandBuffers[ImgIdx]);
    Model->Draw(CommandBuffers[ImgIdx]);
    
    vkCmdEndRenderPass(CommandBuffers[ImgIdx]);
    if (vkEndCommandBuffer(CommandBuffers[ImgIdx]) !=  VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end command buffer");
    }
}

void Application::DrawFrame()
{
    // Retrieve the information about the frame to draw next
    uint32_t ImageIdx;
    VkResult Result = SwapChain->acquireNextImage(&ImageIdx);
    
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

void Application::LoadModels()
{
    // Initializes position and color for each vertex
    std::vector<Vertex> Vertices =
    { {{0.f, -0.5f}, {1.f, 0.f, 0.f}}
    , {{0.5f, 0.5f}, {0.f, 1.f, 0.f}}
    , {{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}};
    
    Model = std::make_unique<LveModel>(Device, Vertices);
}

}
