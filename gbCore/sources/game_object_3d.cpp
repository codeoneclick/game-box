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

namespace gb
{
    game_object_3d::game_object_3d()
    {
        ces_entity::add_deferred_component_constructor<ces_transformation_3d_component>();
        
        position.setter([=](const glm::vec3& position) {
            auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            transformation_component->set_position(position);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        position.getter([=]() {
            auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            return transformation_component->get_position();
        });
        
        rotation.setter([=](const glm::vec3& rotation) {
            auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            transformation_component->set_rotation(rotation);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        rotation.getter([=]() {
            auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            return transformation_component->get_rotation();
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
