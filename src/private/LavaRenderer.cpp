//
//  LavaRenderer.cpp
//  lava
//
//  Created by Giorgio Gamba on 10/01/25.
//

#include "LavaRenderer.hpp"

namespace lava
{

#pragma region Lifecycle

LavaRenderer::LavaRenderer(LavaWindow& InWindow, LavaDevice& InDevice)
: Window(InWindow)
, Device(InDevice)
, bIsFrameStarted(false)
, CurrFrameIdx(0)
{
    RecreateSwapChain();
    CreateCommandBuffers();
}

LavaRenderer::~LavaRenderer()
{
    freeCommandBuffers();
}

#pragma endregion

#pragma region Frame drawing

int LavaRenderer::GetFrameIdx() const
{
    assert(bIsFrameStarted && "Frame not in progress");
    return CurrFrameIdx;
}

VkCommandBuffer LavaRenderer::StartDrawFrame()
{
    assert(!bIsFrameStarted && "Frame already drawing");
    
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
    
    if (vkBeginCommandBuffer(CurrCommandBuffer, &BeginInfo) != VK_SUCCESS)
    {
        throw new std::runtime_error("Failed to begin recording command buffer");
    }
    
    return CurrCommandBuffer;
}

void LavaRenderer::EndDrawFrame()
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
    CurrFrameIdx = (CurrFrameIdx + 1) & LavaSwapChain::MAX_FRAMES_IN_FLIGHT;
}

#pragma endregion

#pragma region Swap Chain

void LavaRenderer::RecreateSwapChain()
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
        SwapChain = std::make_unique<LavaSwapChain>(Device, Extent);
    }
    else
    {
        // Transfers information from current swap chain to old one
        std::shared_ptr<LavaSwapChain> OldSwapChain = std::move(SwapChain);
        
        SwapChain = std::make_unique<LavaSwapChain>(Device, Extent, OldSwapChain);
        
        if (!OldSwapChain || !OldSwapChain->CompareSwapFormats(*SwapChain.get()))
            throw std::runtime_error("Swap chain format has changed");
        
        if (SwapChain->imageCount() != CommandBuffers.size())
        {
            freeCommandBuffers();
            CreateCommandBuffers();
        }
    }
    
    // #TODO to fix
}

void LavaRenderer::StartSwapChainRenderPass(VkCommandBuffer& CommandBuffer)
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
    
    // Viewport Transform
    // Basically a postprocess viewport applied to the vertex shader output
    // If of fixed size, then the objects will change acconrding to the window size
    // this because the XY dimensions of the viewing volume are stretched in order
    // to fit the viewport dimensions
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

void LavaRenderer::EndSwapChainRenderPass(VkCommandBuffer& CommandBuffer)
{
    assert(bIsFrameStarted && "no frame is currently drawing");
    assert(CommandBuffer == GetCurrentCommandBuffer() && "Can't render on a different frame");
    
    vkCmdEndRenderPass(CommandBuffer);
}

#pragma endregion

#pragma region Command Buffers

VkCommandBuffer LavaRenderer::GetCurrentCommandBuffer() const
{
    assert(IsFrameInProgress() && "The frame has not started");
    //assert((CommandBuffers.size() > CurrImageIdx && CurrImageIdx >= 0) && "Current Image Index is not valid");
    
    return CommandBuffers[CurrFrameIdx];
}

void LavaRenderer::CreateCommandBuffers()
{
    // Create a number of buffers equal to the number of drawable frames
    CommandBuffers.resize(LavaSwapChain::MAX_FRAMES_IN_FLIGHT);
    
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

void LavaRenderer::freeCommandBuffers()
{
    vkFreeCommandBuffers(Device.device(), Device.getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
    CommandBuffers.clear();
}

#pragma endregion

}
