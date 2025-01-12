//
//  LveRenderer.cpp
//  lava
//
//  Created by Giorgio Gamba on 10/01/25.
//

#include "LveRenderer.hpp"

namespace Lve
{

#pragma region Lifecycle

LveRenderer::LveRenderer(LveWindow& InWindow, LveDevice& InDevice)
: Window(InWindow)
, Device(InDevice)
{
    RecreateSwapChain();
    CreateCommandBuffers();
}

LveRenderer::~LveRenderer()
{
    freeCommandBuffers();
}

#pragma endregion

#pragma region Frame drawing

VkCommandBuffer LveRenderer::StartDrawFrame()
{
    assert(!IsFrameInProgress() && "Frame already drawing");
    
    // Retrieve the information about the frame to draw next
    VkResult Result = SwapChain->acquireNextImage(&CurrImageIdx);
    
    // Checks if the swap chain is still valid after resizing and before drawing
    if (Result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return nullptr;
    }
    
    if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire next swap chain image");
    }
    
    bIsFrameStarted = true;
    
    VkCommandBuffer CurrCommandBuffer = GetCurrentCommandBuffer();
    
    VkCommandBufferBeginInfo BeginInfo{};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(CommandBuffers[CurrImageIdx], &BeginInfo) != VK_SUCCESS)
    {
        throw new std::runtime_error("Failed to begin recording command buffer");
    }
    
    return CurrCommandBuffer;
}

void LveRenderer::EndDrawFrame()
{
    assert(IsFrameInProgress() && "No frame is currently drawing");
    
    VkCommandBuffer CurrCommandBuffer = GetCurrentCommandBuffer();
    
    if (vkEndCommandBuffer(CurrCommandBuffer) !=  VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end command buffer");
    }
    
    // Provides commands to the device graphics queue (makes also sync)
    const auto Result = SwapChain->submitCommandBuffers(&CurrCommandBuffer, &CurrImageIdx);
    
    if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || Window.WasWindowResized())
    {
        Window.ResetWindowResizedStatus();
        RecreateSwapChain();
    }
    
    bIsFrameStarted = false;
}

#pragma endregion

#pragma region Swap Chain

void LveRenderer::RecreateSwapChain()
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
    
    // #TODO to fix
}

void LveRenderer::StartSwapChainRenderPass(VkCommandBuffer& CommandBuffer)
{
    assert(bIsFrameStarted && "no frame is currently drawing");
    assert(CommandBuffer == GetCurrentCommandBuffer() && "Can't render on a different frame");
    
    VkRenderPassBeginInfo RenderPassBeginInfo{};
    RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    RenderPassBeginInfo.renderPass = SwapChain->getRenderPass();
    RenderPassBeginInfo.framebuffer = SwapChain->getFrameBuffer(CurrImageIdx);
    
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
    vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkViewport Viewport{};
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = static_cast<float>(SwapChain->getSwapChainExtent().width);
    Viewport.height = static_cast<float>(SwapChain->getSwapChainExtent().height);
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;
    vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);
    
    VkRect2D Scissor{{0, 0}, SwapChain->getSwapChainExtent()};
    vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);
}

void LveRenderer::EndSwapChainRenderPass(VkCommandBuffer& CommandBuffer)
{
    assert(bIsFrameStarted && "no frame is currently drawing");
    assert(CommandBuffer == GetCurrentCommandBuffer() && "Can't render on a different frame");
    
    vkCmdEndRenderPass(CommandBuffer);
}

#pragma endregion

#pragma region Command Buffers

VkCommandBuffer LveRenderer::GetCurrentCommandBuffer() const
{
    assert(IsFrameInProgress() && "The frame has not started");
    assert((CommandBuffers.size() > CurrImageIdx && CurrImageIdx >= 0) && "Current Image Index is not valid");
    
    return CommandBuffers[CurrImageIdx];
}

void LveRenderer::CreateCommandBuffers()
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

void LveRenderer::freeCommandBuffers()
{
    vkFreeCommandBuffers(Device.device(), Device.getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
    CommandBuffers.clear();
}

#pragma endregion

}