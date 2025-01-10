//
//  GameObject.hpp
//  lava
//
//  Created by Giorgio Gamba on 09/01/25.
//

#pragma once

#include "LveModel.hpp"

namespace lve
{

struct Transform2DComponent
{
    glm::vec2 Translation{}; // Represents offsets up/down/left/right

    glm::mat2 mat2() { return glm::mat2{1.f}; }
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
    
private:
    
    LveGameObject(id_t ObjectId)
    : Id{ObjectId}
    {}
    
    id_t Id;
    
    std::shared_ptr<LveModel> Model{};
    glm::vec3 Color{};
    
    Transform2DComponent Transform2D{};
    
};

}
