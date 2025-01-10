//
//  GameObject.hpp
//  lava
//
//  Created by Giorgio Gamba on 09/01/25.
//

#pragma once

#include "LveModel.hpp"

namespace Lve
{

/**
 
 The transform matrix's columns represent the unit vectors of a point in the space. In this 2D Matrix, the first column represents i and the second j
 This means that, in order to represent the coordinates of a rotated unit vector, we can use cos and sin of the rotation angle to get the rotation of the model
 
 */
struct Transform2DComponent
{
    // Currently rotation is represented through an explicit offset application
    glm::vec2 Translation{}; // Represents offsets up/down/left/right
    glm::vec2 Scale{1.f};
    float RotationAngle;
    
    glm::mat2 mat2() {
    
        // Computate Rotatio matrix
        const float s = glm::sin(RotationAngle);
        const float c = glm::cos(RotationAngle);
        
        // NOTE: Positive Y axis in Vulkan is pointing dowm
        const glm::mat2 RotationMatrix
        {
            {c, s}, // Represents the (x, y) coordinates of the rotated basis vector i
            {-s, c} // Represents the (x, y) coordinates of the rotated basis vector j
        };
        
        // Defined col by col
        const glm::mat2 ScaleMatrix
        {
            {Scale.x, 0.f},
            {0.f, Scale.y}
        };
        
        return RotationMatrix * ScaleMatrix;
        
        
    }
};

// Represents eveything that is in game, with a set of properties
class LveGameObject
{
    
public:
    
    using id_t = unsigned int;
    
    static LveGameObject CreateGameObject()
    {
        static id_t CurrentId = 0; // Keeps track of all the built game objs
        return LveGameObject{CurrentId++};
    }
    
    // Remove copy operators
    LveGameObject(LveGameObject&) = delete;
    LveGameObject& operator=(LveGameObject&) = delete;
    
    // Add move operators
    LveGameObject(LveGameObject&&) = default;
    LveGameObject& operator=(LveGameObject&&) = default;
    
    id_t GetId() const { return Id; }
    
    std::shared_ptr<LveModel> GetModel() const { return Model; }
    void SetModel(const std::shared_ptr<LveModel>& InModel) { Model = InModel; }
    
    glm::vec3 GetColor() const { return Color; }
    void SetColor(const glm::vec3& InColor) { Color = InColor; }
    
    Transform2DComponent Transform2D{};
    
private:
    
    LveGameObject(id_t ObjectId)
    : Id{ObjectId}
    {}
    
    id_t Id;
    
    std::shared_ptr<LveModel> Model{};
    glm::vec3 Color{};
    
};

}
