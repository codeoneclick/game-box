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
#include "ces_transformation_extension.h"

namespace gb
{
    game_object::game_object()
    {
        ces_transformation_component_shared_ptr transformation_component = std::make_shared<ces_transformation_component>();
        ces_entity::add_component(transformation_component);
        
        position.setter([=](const glm::vec2& position) {
            if(m_tag == "pt_03")
            {
                std::cout<<"pt_03"<<std::endl;
            }
            unsafe_get_transformation_component_from_this->set_position(position);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        position.getter([=]() {
           return unsafe_get_transformation_component_from_this->get_position();
        });
        
        rotation.setter([=](f32 rotation) {
            unsafe_get_transformation_component_from_this->set_rotation(rotation);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        rotation.getter([=]() {
            return unsafe_get_transformation_component_from_this->get_rotation();
        });
        
        scale.setter([=](const glm::vec2& scale) {
            unsafe_get_transformation_component_from_this->set_scale(scale);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        scale.getter([=]() {
            return unsafe_get_transformation_component_from_this->get_scale();
        });
        
        z_order.setter([=](f32 z_order) {
            unsafe_get_transformation_component_from_this->set_z_order(z_order);
        });
        
        z_order.getter([=]() {
            return unsafe_get_transformation_component_from_this->get_z_order();
        });
        
        size.setter([=](const glm::vec2& size) {
            unsafe_get_transformation_component_from_this->set_scale(size);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
        size.getter([=]() {
            return unsafe_get_transformation_component_from_this->get_scale();
        });
        
        bound.getter([=]() {
            return glm::vec4(0.f);
        });
    }
    
    game_object::~game_object()
    {
        
    }
    
    void game_object::update_z_order_recursively(const ces_entity_shared_ptr& entity, f32& z_order)
    {
        ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
        if(transformation_component)
        {
            transformation_component->set_z_order(z_order);
        }
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            z_order = z_order + ces_transformation_component::k_z_order_step;
            game_object::update_z_order_recursively(child, z_order);
        }
    }
    
    void game_object::add_child(const ces_entity_shared_ptr& child)
    {
        ces_entity::add_child(child);
        ces_transformation_component_shared_ptr transformation_component = child->get_component<ces_transformation_component>();
        f32 z_order = 0;
        ces_entity_shared_ptr parent = game_object::parent;
        while (parent) {
            parent = parent->parent;
        }
        game_object::update_z_order_recursively(parent ? parent : shared_from_this(), z_order);
        ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
    }
}
