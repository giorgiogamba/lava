//
//  GameObject.hpp
//  lava
//
//  Created by Giorgio Gamba on 09/01/25.
//

#pragma once

#include "LavaModel.hpp"

#include "LavaTypes.hpp"

namespace Lava
{

// Represents eveything that is in game, with a set of properties
class LavaGameObject
{
    
public:
    
    using id_t = unsigned int;
    
    static LavaGameObject CreateGameObject()
    {
        static id_t CurrentId = 0; // Keeps track of all the built game objs
        return LavaGameObject{CurrentId++};
    }
    
    // Remove copy operators
    LavaGameObject(LavaGameObject&) = delete;
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
