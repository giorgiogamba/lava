//
//  LveRenderer.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

// System includes
#include <stdio.h>
#include <string>
#include <memory>
#include <cassert>

// Local includes
#include "LveWindow.hpp"
#include "LveSwapChain.hpp"

namespace Lve {

class LveRenderer
{
    
#pragma region Lifecycle
    
public:
    
    LveRenderer(LveWindow& InWindow, LveDevice& InDevice);
    ~LveRenderer();
    
    LveRenderer(const LveRenderer&) = delete;
    LveRenderer& operator=(const LveRenderer&) = delete;
    
private:
    
    // #TODO remove this absolute path
    std::string absPathPrefix = "/Users/giorgiogamba/Documents/Projects/lava/lava/lava/";
    
    LveWindow& Window;
    
    LveDevice& Device;
    
#pragma endregion
    
#pragma region Frame Drawing
    
public:
    
    VkCommandBuffer StartDrawFrame();
    void EndDrawFrame();
    
private:
    
    bool IsFrameInProgress() const { return bIsFrameStarted; }
    
    uint32_t CurrImageIdx;
    
    bool bIsFrameStarted = false;
    
#pragma endregion
    
#pragma region Swap Chain

public:
    
    void StartSwapChainRenderPass(VkCommandBuffer& CommandBuffer);
    void EndSwapChainRenderPass(VkCommandBuffer& CommandBuffer);
    
    VkRenderPass GetSwapChainRenderPass() const { return SwapChain->getRenderPass(); }
    
private:
    
    void RecreateSwapChain();
    
    std::unique_ptr<LveSwapChain> SwapChain;
    
#pragma endregion
    
#pragma region Command Buffers
    
public:
    
    VkCommandBuffer GetCurrentCommandBuffer() const;
    
private:
    
    void CreateCommandBuffers();
    
    void freeCommandBuffers();
    
    std::vector<VkCommandBuffer> CommandBuffers;
    
#pragma endregion
    
};

}

