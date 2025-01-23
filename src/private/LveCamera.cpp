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

void LveCamera::SetViewDirection(const glm::vec3& Position, const glm::vec3& FrontDirection, const glm::vec3& UpDirection /*= glm::vec3{0.f, -1.f, 0.f}*/)
{
    // Create ortho normal basis: 3 vectors orthogonal to each other that define camera orientation in space
    // Together they define the rotation matrix of the camera
    const glm::vec3 w{glm::normalize(FrontDirection)};
    const glm::vec3 u{glm::normalize(glm::cross(w, UpDirection))};
    const glm::vec3 v{glm::cross(w, u)};

    PopulateViewMatrix(Position, u, v, w);
}

void LveCamera::SetViewTarget(const glm::vec3& Position, const glm::vec3& Target, const glm::vec3& UpDirection /*= glm::vec3{0.f, -1.f, 0.f}*/)
{
    // Target - Position represents the vector that points from Position to Target
    SetViewDirection(Position, Target - Position, UpDirection);
}

void LveCamera::SetViewYX(glm::vec3 Position, glm::vec3 Rotation)
{
    // To build View Matrix we combine Rotation matrix INVERSE and translation back to the origin
    // We use ivnerse rotation because we want to move the camera direction back to the canonical direction -> opposite of model transform
    
    // Classic rotation matrix creation. We could use also negative angles to build inverse matrix
    const float c3 = glm::cos(Rotation.z);
    const float s3 = glm::sin(Rotation.z);
    const float c2 = glm::cos(Rotation.x);
    const float s2 = glm::sin(Rotation.x);
    const float c1 = glm::cos(Rotation.y);
    const float s1 = glm::sin(Rotation.y);
    
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    
    PopulateViewMatrix(Position, u, v, w);
}

void LveCamera::PopulateViewMatrix(const glm::vec3 Position, const glm::vec3& u, const glm::vec3& v, const glm::vec3& w)
{
    ViewMatrix = glm::mat4{1.f};
    ViewMatrix[0][0] = u.x;
    ViewMatrix[1][0] = u.y;
    ViewMatrix[2][0] = u.z;
    ViewMatrix[0][1] = v.x;
    ViewMatrix[1][1] = v.y;
    ViewMatrix[2][1] = v.z;
    ViewMatrix[0][2] = w.x;
    ViewMatrix[1][2] = w.y;
    ViewMatrix[2][2] = w.z;
    ViewMatrix[3][0] = -glm::dot(u, Position);
    ViewMatrix[3][1] = -glm::dot(v, Position);
    ViewMatrix[3][2] = -glm::dot(w, Position);
}

}
