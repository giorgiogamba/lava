//
//  LveCamera.hpp
//  lava
//
//  Created by Giorgio Gamba on 20/01/25.
//

#pragma once

#define GLM_FORCE_RADIANS // expects angles to be defined in radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lve
{

class LveCamera
{
public:
    
    void SetOrthoProjection(const float left, const float right, const float top, const float bottom, const float near, const float far);
    
    void SetPerspectiveProjection(const float fov, const float aspectRatio, const float near, const float far);
    
    glm::mat4 GetProjectionMat() const { return ProjectionMat; }
    
private:
    
    // Projection matrix used to draw on screen indipendently from the type of projection used
    glm::mat4 ProjectionMat{1.f};
};

}
