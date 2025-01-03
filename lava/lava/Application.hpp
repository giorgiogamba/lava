//
//  Application.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <string>
#include <memory>

#endif /* Application_hpp */

#include "LveWindow.hpp"
#include "LvePipeline.hpp"
#include "LveSwapChain.hpp"

namespace Lve {

static constexpr int WIDTH = 800;
static constexpr int HEIGTH = 800;

class Application
{
public:
    
    Application();
    ~Application();
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    void Run();
    
private:
    
    void CreatePipelineLayout();
    void CreatePipeline();
    void CreateCommandBuffers();
    
    void DrawFrame();
    
    // #TODO remove this absolute path
    std::string absPathPrefix = "/Users/giorgiogamba/Documents/Projects/lava/lava/lava/";
    
    LveWindow Window{WIDTH, HEIGTH, "TEST"};
    
    LveDevice Device{Window};
    
    LveSwapChain SwapChain{Device, Window.getExtent()};
    
    std::unique_ptr<LvePipeline> Pipeline;
    VkPipelineLayout PipelineLayout;
    std::vector<VkCommandBuffer> CommandBuffers;
};

}
