//
//  game_object.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_object.h"
#include "ces_transformation_component.h"
#include "ces_scene_component.h"
#include "camera.h"

namespace gb
{
    game_object::game_object()
    {
        ces_transformation_component_shared_ptr transformation_component = std::make_shared<ces_transformation_component>();
        ces_entity::add_component(transformation_component);
        
        position.setter([=](const glm::vec2& position) {
            unsafe_get_transformation_component_from_this->set_position(position);
        });
        position.getter([=]() {
           return unsafe_get_transformation_component_from_this->get_position();
        });
        
        rotation.setter([=](f32 rotation) {
            unsafe_get_transformation_component_from_this->set_rotation(rotation);
        });
        rotation.getter([=]() {
            return unsafe_get_transformation_component_from_this->get_rotation();
        });
        
        scale.setter([=](const glm::vec2& scale) {
            unsafe_get_transformation_component_from_this->set_scale(scale);
        });
        scale.getter([=]() {
            return unsafe_get_transformation_component_from_this->get_scale();
        });
        
        bound.getter([=]() {
            return glm::vec4(0.f);
        });
    }
    
    game_object::~game_object()
    {
        
    }
}