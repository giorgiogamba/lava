//
//  LavaRenderer.hpp
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
#include "LavaWindow.hpp"
#include "LavaSwapChain.hpp"

namespace lava {

class LavaRenderer
{
    
#pragma region Lifecycle
    
public:
    
    LavaRenderer(LavaWindow& InWindow, LavaDevice& InDevice);
    ~LavaRenderer();
    
    LavaRenderer(const LavaRenderer&) = delete;
    LavaRenderer& operator=(const LavaRenderer&) = delete;
    
private:
    
    LavaWindow& Window;
    
    LavaDevice& Device;
    
#pragma endregion
    
#pragma region Frame Drawing
    
public:
    
    VkCommandBuffer StartDrawFrame();
    void EndDrawFrame();
    
private:
    
    bool IsFrameInProgress() const { return bIsFrameStarted; }
    
    int GetFrameIdx() const;
    
    uint32_t CurrImageIdx;
    int CurrFrameIdx; // Goes from 0 to MaxFramesInFlight
    
    bool bIsFrameStarted;
    
#pragma endregion
    
#pragma region Swap Chain

public:
    
    void StartSwapChainRenderPass(VkCommandBuffer& CommandBuffer);
    void EndSwapChainRenderPass(VkCommandBuffer& CommandBuffer);
    
    VkRenderPass GetSwapChainRenderPass() const { return SwapChain->getRenderPass(); }
    
    float GetAspectRatio() const { return SwapChain->extentAspectRatio(); }
    
private:
    
    void RecreateSwapChain();
    
    std::unique_ptr<LavaSwapChain> SwapChain;
    
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

