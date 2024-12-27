//
//  Application.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>

#endif /* Application_hpp */

#include "LveWindow.hpp"

namespace Lve {

static constexpr int WIDTH = 800;
static constexpr int HEIGTH = 800;

class Application
{
public:
    
    void Run();
    
private:
    
    LveWindow LveWindow{WIDTH, HEIGTH, "TEST"};
    
};

}
