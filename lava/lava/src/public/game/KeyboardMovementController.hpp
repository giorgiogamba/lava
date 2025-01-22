//
//  KeyboardMovementController.hpp
//  lava
//
//  Created by Giorgio Gamba on 21/01/25.
//

#pragma once

#include "LveGameObject.hpp"
#include "LveWindow.hpp"

namespace Lve
{

#pragma region Types

struct KeyMappings
{
    const int MoveLeft = GLFW_KEY_A;
    const int MoveRight = GLFW_KEY_D;
    const int MoveForward = GLFW_KEY_W;
    const int MoveBackward = GLFW_KEY_S;
    const int MoveUp = GLFW_KEY_E;
    const int MoveDown = GLFW_KEY_Q;
    const int LookLeft = GLFW_KEY_LEFT;
    const int LookRight = GLFW_KEY_RIGHT;
    const int LookUp = GLFW_KEY_UP;
    const int LookDown = GLFW_KEY_DOWN;
};

#pragma endregion

class KeyboardMovementController
{
    
public:
    
    KeyboardMovementController();
    
    void MoveInPlaneXZ(GLFWwindow* Window, const float DeltaTime, LveGameObject& GameObject);
    
    KeyMappings Keys{};
    
    float MoveSpeed;
    float LookSpeed;
    
private:
    
    bool IsVectorValid(const glm::vec3 InVector) const;
    
    void UpdateRotationViewTransform(GLFWwindow* Window, glm::vec3& Rotate);
    
    void UpdateMovementDirectionTransform(GLFWwindow* Window, const float Yaw, glm::vec3& MovementDirection);
    
};

}
