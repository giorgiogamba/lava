//
//  KeyboardMovementController.cpp
//  lava
//
//  Created by Giorgio Gamba on 21/01/25.
//

#include "KeyboardMovementController.hpp"

namespace lava
{

KeyboardMovementController::KeyboardMovementController()
    : MoveSpeed(3.f)
    , LookSpeed(1.5f)
{}

void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* Window, const float DeltaTime, LavaGameObject& GameObject)
{
    glm::vec3 Rotate{0.f}; // Stores the current rotation inputs
    UpdateRotationViewTransform(Window, Rotate);
    
    // Check normalize different from 0 in order to avoid problems with 0 normalization
    // Check if the dot product is bigger than 0 means that the norm2 (length) of the vector is bigger than 0
    if (IsVectorValid(Rotate))
        // Normalize rotation in order to get uniform rotative movement in all directions
        GameObject.Transform.Rotation += LookSpeed * DeltaTime * glm::normalize(Rotate);
    
    GameObject.Transform.Rotation.x = glm::clamp(GameObject.Transform.Rotation.x, -1.5f, 1.5f);
    
    // Remember that Y represents the vertical axis (with -y pointing up), while Z represents depth
    GameObject.Transform.Rotation.y = glm::mod(GameObject.Transform.Rotation.y, glm::two_pi<float>()); // Makes mod repetition in order to avoid overflow
    
    glm::vec3 MovementDirection{0.f};
    UpdateMovementDirectionTransform(Window, GameObject.Transform.Rotation.y, MovementDirection);
    if (IsVectorValid(MovementDirection))
        GameObject.Transform.Translation += MoveSpeed * DeltaTime * glm::normalize(MovementDirection);
}

bool KeyboardMovementController::IsVectorValid(const glm::vec3 InVector) const
{
    return glm::dot(InVector, InVector) > std::numeric_limits<float>::epsilon();
}

void KeyboardMovementController::UpdateRotationViewTransform(GLFWwindow* Window, glm::vec3& Rotate)
{
    if (glfwGetKey(Window, Keys.LookRight) == GLFW_PRESS)
        Rotate.y += 1.f;
    if (glfwGetKey(Window, Keys.LookLeft) == GLFW_PRESS)
        Rotate.y -= 1.f;
    if (glfwGetKey(Window, Keys.LookUp) == GLFW_PRESS)
        Rotate.x += 1.f;
    if (glfwGetKey(Window, Keys.LookDown) == GLFW_PRESS)
        Rotate.x -= 1.f;
}

void KeyboardMovementController::UpdateMovementDirectionTransform(GLFWwindow* Window, const float Yaw, glm::vec3& MovementDirection)
{
    // Supposed this is the direction on the XZ plane
    const glm::vec3 ForwardDirection{sin(Yaw), 0.f, cos(Yaw)};
    const glm::vec3 RightDirection{ForwardDirection.z, 0.f, -ForwardDirection.x};
    const glm::vec3 UpDirection{0.f, -1.f, 0.f};
    
    if (glfwGetKey(Window, Keys.MoveForward) == GLFW_PRESS)
        MovementDirection += ForwardDirection;
    if (glfwGetKey(Window, Keys.MoveBackward) == GLFW_PRESS)
        MovementDirection -= ForwardDirection;
    if (glfwGetKey(Window, Keys.MoveLeft) == GLFW_PRESS)
        MovementDirection -= RightDirection;
    if (glfwGetKey(Window, Keys.MoveRight) == GLFW_PRESS)
        MovementDirection += RightDirection;
    if (glfwGetKey(Window, Keys.MoveUp) == GLFW_PRESS)
        MovementDirection += UpDirection;
    if (glfwGetKey(Window, Keys.MoveDown) == GLFW_PRESS)
        MovementDirection -= UpDirection;
}

}
