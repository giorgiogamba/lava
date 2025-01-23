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

/*
 Camera Transform: composed by 2 steps:
 1. Makes the camera move to the origin -> translation
 2. Orient the camera to negative Z -> rotation
 
 Basically it is like the objects are moving according to the camera frustum
 */
class LveCamera
{
public:
    
    void SetOrthoProjection(const float left, const float right, const float top, const float bottom, const float near, const float far);
    
    void SetPerspectiveProjection(const float fov, const float aspectRatio, const float near, const float far);
    
    void SetViewDirection(const glm::vec3& Position, const glm::vec3& FrontDirection, const glm::vec3& UpDirection = glm::vec3{0.f, -1.f, 0.f});
    
    /** Used to lock camera onto a specifi point in space (Target) **/
    void SetViewTarget(const glm::vec3& Position, const glm::vec3& Target, const glm::vec3& UpDirection = glm::vec3{0.f, -1.f, 0.f});
    
    /** Specifies camera orientation in Euler angles **/
    void SetViewYX(glm::vec3 Position, glm::vec3 Rotation);
    
    void PopulateViewMatrix(const glm::vec3 Position, const glm::vec3& u, const glm::vec3& v, const glm::vec3& w);
    
    glm::mat4 GetProjectionMat() const { return ProjectionMat; }
    
    glm::mat4 GetViewMat() const { return ViewMatrix; }
    
private:
    
    // Projection matrix used to draw on screen indipendently from the type of projection used
    glm::mat4 ProjectionMat{1.f};
    
    glm::mat4 ViewMatrix{1.f};
};

}
