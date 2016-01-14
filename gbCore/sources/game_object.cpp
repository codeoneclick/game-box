//
//  game_object.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_object.h"
#include "ces_transformation_component.h"

namespace gb
{
    game_object::game_object()
    {
        ces_transformation_component_shared_ptr transformation_component = std::make_shared<ces_transformation_component>();
        ces_entity::add_component(transformation_component);
    }
    
    game_object::~game_object()
    {
        
    }
    
    void game_object::set_position(const glm::vec2& position)
    {
        unsafe_get_transformation_component_from_this->set_position(position);
    }
    
    glm::vec2 game_object::get_position() const
    {
        return unsafe_get_transformation_component_from_this->get_position();
    }
    
    void game_object::set_rotation(f32 rotation)
    {
        unsafe_get_transformation_component_from_this->set_rotation(rotation);
    }
    
    f32 game_object::get_rotation() const
    {
        return unsafe_get_transformation_component_from_this->get_rotation();
    }
    
    void game_object::set_scale(const glm::vec2& scale)
    {
        unsafe_get_transformation_component_from_this->set_scale(scale);
    }
    
    glm::vec2 game_object::get_scale() const
    {
        return unsafe_get_transformation_component_from_this->get_scale();
    }
    
    glm::vec2 game_object::get_min_bound() const
    {
        return glm::vec2(0.f);
    }
    
    glm::vec2 game_object::get_max_bound() const
    {
        return glm::vec2(0.f);
    }
    
    glm::vec4 game_object::get_bound_frame() const
    {
        return glm::vec4(0.f);
    }
}