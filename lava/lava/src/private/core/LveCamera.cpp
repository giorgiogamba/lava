//
//  LveCamera.cpp
//  lava
//
//  Created by Giorgio Gamba on 20/01/25.
//

#include "LveCamera.hpp"

#include <cassert>
#include <limits>

namespace Lve
{

void LveCamera::SetOrthoProjection(const float left, const float right, const float top, const float bottom, const float near, const float far)
{
    ProjectionMat = glm::mat4{1.f};
    ProjectionMat[0][0] = 2.f / (right - left);
    ProjectionMat[1][1] = 2.f / (bottom - top);
    ProjectionMat[2][2] = 1.f / (far - near);
    ProjectionMat[3][0] = -(right + left) / (right - left);
    ProjectionMat[3][1] = -(bottom + top) / (bottom - top);
    ProjectionMat[3][2] = -near / (far - near);
}

void LveCamera::SetPerspectiveProjection(const float fov, const float aspectRatio, const float near, const float far)
{
    // Check arguments validity
    assert(glm::abs(aspectRatio - std::numeric_limits<float>::epsilon()) > 0.f);
    
    const float HalfFovTan = tan(fov / 2.f);
    
    ProjectionMat = glm::mat4{0.f};
    ProjectionMat[0][0] = 1.f / (aspectRatio * HalfFovTan);
    ProjectionMat[1][1] = 1.f / (HalfFovTan);
    ProjectionMat[2][2] = far / (far - near);
    ProjectionMat[2][3] = 1.f;
    ProjectionMat[3][2] = -(far * near) / (far - near);
 }

}
