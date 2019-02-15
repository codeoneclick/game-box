//
//  game_object_3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "game_object_3d.h"
#include "ces_transformation_3d_component.h"
#include "ces_transformation_extension.h"
#include "ces_box2d_body_component.h"

namespace gb
{
    game_object_3d::game_object_3d()
    {
        ces_entity::add_deferred_component_constructor<ces_transformation_3d_component>();
        
        position.setter([=](const glm::vec3& position) {
            auto box2d_body_component = ces_entity::get_component<ces_box2d_body_component>();
            if(box2d_body_component && box2d_body_component->is_applied)
            {
                box2d_body_component->position = glm::vec2(position.x, position.y);
            }
            else
            {
                auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
                transformation_component->set_position(position);
            }
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        position.getter([=]() {
            auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            auto position = transformation_component->get_position();
            auto box2d_body_component = ces_entity::get_component<ces_box2d_body_component>();
            if(box2d_body_component && box2d_body_component->is_applied)
            {
                glm::vec2 box2d_position = box2d_body_component->position;
                position.x = box2d_position.x;
                position.z = box2d_position.y;
            }
           
            return position;
        });
        
        rotation.setter([=](const glm::vec3& rotation) {
            auto box2d_body_component = ces_entity::get_component<ces_box2d_body_component>();
            if(box2d_body_component && box2d_body_component->is_applied)
            {
                box2d_body_component->rotation = rotation.y;
            }
            else
            {
                auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
                transformation_component->set_rotation(rotation);
            }
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        rotation.getter([=]() {
            auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            auto rotation = transformation_component->get_rotation();
            auto box2d_body_component = ces_entity::get_component<ces_box2d_body_component>();
            if(box2d_body_component && box2d_body_component->is_applied)
            {
                f32 box2d_rotation = box2d_body_component->rotation;
                rotation.y = box2d_rotation;
            }

            return rotation;
        });
        
        scale.setter([=](const glm::vec3& scale) {
            auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            transformation_component->set_scale(scale);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        scale.getter([=]() {
            auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            return transformation_component->get_scale();
        });
    }
    
    game_object_3d::~game_object_3d()
    {
        
    }
}
