//
//  GameObject.hpp
//  lava
//
//  Created by Giorgio Gamba on 09/01/25.
//

#pragma once

#include "LavaModel.hpp"

#include <unordered_map>

#pragma region Transforms

/**
 The transform matrix's columns represent the unit vectors of a point in the space. In this 2D Matrix, the first column represents i and the second j
 This means that, in order to represent the coordinates of a rotated unit vector, we can use cos and sin of the rotation angle to get the rotation of the model
 */
struct Transform2DComponent
{
    // Currently translation is represented through an explicit offset application
    glm::vec2 Translation{}; // Represents offsets up/down/left/right
    glm::vec2 Scale{1.f};
    float RotationAngle;
    
    glm::mat2 mat2()
    {
    
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

// 3D Transform
struct TransformComponent
{
    glm::vec3 Translation{}; // Represents offsets up/down/left/right
    glm::vec3 Scale{1.f, 1.f, 1.f};
    
    // Use YXZ rotation representation (Tait-Bryan representation)
    glm::vec3 Rotation{};
    
    // Classic implementation
    // glm::mat4 mat4()
    // {
    //    // This creates a 4x4 Identity Matrix, with applied translation
    //     glm::mat4 Transform = glm::translate(glm::mat4{1.f}, Translation);
        
    //     // Classic YXZ Tait-Bryan representation
    //     Transform = glm::rotate(Transform, Rotation.y, {0.f, 1.f, 0.f});
    //     Transform = glm::rotate(Transform, Rotation.x, {1.f, 0.f, 0.f});
    //     Transform = glm::rotate(Transform, Rotation.z, {0.f, 0.f, 1.f});
        
    //     Transform = glm::scale(Transform, Scale);
        
    //     return Transform;
    // }
    
    // Optimized representation
    // This basically makes the Y * X * Z matrix computation and gets the formula for each resulting matrix position
    // making the same calculus but in a more efficient way because already knowing the components
    glm::mat4 mat4()
    {
        const float c3 = glm::cos(Rotation.z);
        const float s3 = glm::sin(Rotation.z);
        const float c2 = glm::cos(Rotation.x);
        const float s2 = glm::sin(Rotation.x);
        const float c1 = glm::cos(Rotation.y);
        const float s1 = glm::sin(Rotation.y);
        
        return glm::mat4
        {
            {
                Scale.x * (c1 * c3 + s1 * s2 * s3),
                Scale.x * (c2 * s3),
                Scale.x * (c1 * s2 * s3 - c3 * s1),
                0.f
            },
            {
                Scale.y * (c3 * s1 * s2 - c1 * s3),
                Scale.y * (c2 * c3),
                Scale.y * (c1 * c3 * s2 + s1 * s3),
                0.f
            },
            {
                Scale.z * (c2 * s1),
                Scale.z * (-s2),
                Scale.z * (c1 * c2),
                0.f
            },
            {Translation.x, Translation.y, Translation.z, 1.f}
        };
    };

    // A normal matrix is built by R * Sˆ-1
    // This means that we we need the rotation matrix together with the scale matrix with
    // the elements converted by their mutual component (1 / scale)
    glm::mat3 normalMatrix()
    {
        const float c3 = glm::cos(Rotation.z);
        const float s3 = glm::sin(Rotation.z);
        const float c2 = glm::cos(Rotation.x);
        const float s2 = glm::sin(Rotation.x);
        const float c1 = glm::cos(Rotation.y);
        const float s1 = glm::sin(Rotation.y);
        const glm::vec3 InverseScale = 1.f / Scale;
        
        return glm::mat3
        {
            {
                InverseScale.x * (c1 * c3 + s1 * s2 * s3),
                InverseScale.x * (c2 * s3),
                InverseScale.x * (c1 * s2 * s3 - c3 * s1),
            },
            {
                InverseScale.y * (c3 * s1 * s2 - c1 * s3),
                InverseScale.y * (c2 * c3),
                InverseScale.y * (c1 * c3 * s2 + s1 * s3),
            },
            {
                InverseScale.z * (c2 * s1),
                InverseScale.z * (-s2),
                InverseScale.z * (c1 * c2),
            },
        };
    }
};

#pragma endregion

namespace lava
{

// Represents eveything that is in game, with a set of properties
class LavaGameObject
{
    
public:
    
    using id_t = unsigned int;
    using Objects = std::unordered_map<id_t, LavaGameObject>;
    
    static LavaGameObject CreateGameObject()
    {
        static id_t CurrentId = 0; // Keeps track of all the built game objs
        return LavaGameObject{CurrentId++};
    }
    
    LavaGameObject& operator=(LavaGameObject&) = delete;
    
    // Add move operators
    LavaGameObject(LavaGameObject&&) = default;
    LavaGameObject& operator=(LavaGameObject&&) = default;
    
    id_t GetId() const { return Id; }
    
    std::shared_ptr<LavaModel> GetModel() const { return Model; }
    void SetModel(const std::shared_ptr<LavaModel>& InModel) { Model = InModel; }
    
    glm::vec3 GetColor() const { return Color; }
    void SetColor(const glm::vec3& InColor) { Color = InColor; }
    
    TransformComponent Transform{};
    
private:
    
    LavaGameObject(id_t ObjectId)
    : Id{ObjectId}
    {}
    
    id_t Id;
    
    std::shared_ptr<LavaModel> Model{};
    glm::vec3 Color{};
    
};

}
