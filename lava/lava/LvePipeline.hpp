//
//  LvePipeline.hpp
//  lava
//
//  Created by Giorgio Gamba on 27/12/24.
//

#pragma once

#ifndef LvePipeline_hpp
#define LvePipeline_hpp

#include <stdio.h>
#include <string>
#include <vector>

#include "LvePipeline.hpp"

#endif /* LvePipeline_hpp */

namespace Lve {

class LvePipeline
{
public:
    
    LvePipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    
private:
    
    static std::vector<char> readFile(const std::string& filePath);
    
    void createPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    
};

}
