//
//  Application.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <string>

#endif /* Application_hpp */

#include "LveWindow.hpp"
#include "LvePipeline.hpp"

namespace Lve {

static constexpr int WIDTH = 800;
static constexpr int HEIGTH = 800;

class Application
{
public:
    
    void Run();
    
private:
    
    // #TODO remove this absolute path
    std::string absPathPrefix = "/Users/giorgiogamba/Documents/Projects/lava/lava/lava/";
    
    LveWindow LveWindow{WIDTH, HEIGTH, "TEST"};
    
    // Cannot create constexpr strings
    LvePipeline LvePipeline{absPathPrefix + "shaders/vertex_shader.vert.spv", absPathPrefix + "shaders/fragment_shader.frag.spv"};
    
};

}
