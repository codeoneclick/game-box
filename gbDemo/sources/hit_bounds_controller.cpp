//
//  hit_bounds_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "hit_bounds_controller.h"
#include "ces_action_component.h"
#include "ces_box2d_body_component.h"
#include "ces_hit_bounds_component.h"
#include "game_object_2d.h"

namespace game
{
    const f32 hit_bounds_controller::k_hit_bounds_size = 16.f;
    
    hit_bounds_controller::hit_bounds_controller(const gb::ces_entity_shared_ptr& layer,
                                                 const gb::scene_fabricator_shared_ptr& scene_fabricator) :
    m_layer(layer),
    m_scene_fabricator(scene_fabricator)
    {
        ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
    }
    
    hit_bounds_controller::~hit_bounds_controller()
    {
        
    }
    
    void hit_bounds_controller::setup_components()
    {
        auto action_component = ces_entity::get_component<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&hit_bounds_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
    }
    
    void hit_bounds_controller::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        m_hit_bounds.erase(std::remove_if(m_hit_bounds.begin(), m_hit_bounds.end(), [](const std::tuple<gb::game_object_2d_weak_ptr, glm::vec2>& hit_bounds_weak) {
            if(!std::get<0>(hit_bounds_weak).expired())
            {
                auto hit_bounds = std::get<0>(hit_bounds_weak).lock();
                auto box2d_body_component = hit_bounds->get_component<gb::ces_box2d_body_component>();
                auto hit_bounds_component = hit_bounds->get_component<game::ces_hit_bounds_component>();
                if(box2d_body_component->is_contacted && box2d_body_component->is_destructable_on_contact)
                {
                    assert(hit_bounds_component->is_hit_callback_exist());
                    if(hit_bounds_component->is_hit_callback_exist())
                    {
                        hit_bounds_component->get_hit_callback()(hit_bounds);
                    }
                    hit_bounds->remove_from_parent();
                    return true;
                }
                if(box2d_body_component->is_applied)
                {
                    box2d_body_component->linear_velocity = std::get<1>(hit_bounds_weak);
                }
                return false;
            }
            else
            {
                assert(false);
            }
            return true;
        }), m_hit_bounds.end());
    }
    
    void hit_bounds_controller::push_hit_bounds(const gb::ces_entity_shared_ptr& executor, const glm::vec2& position, f32 rotation)
    {
        auto hit_bounds = gb::ces_entity::construct<gb::game_object_2d>();
        hit_bounds->tag = "hit_bounds";
        hit_bounds->visible = false;
        hit_bounds->size = glm::vec2(k_hit_bounds_size);
        
        auto box2d_body_component = std::make_shared<gb::ces_box2d_body_component>();
        box2d_body_component->is_destructable_on_contact = true;
        box2d_body_component->set_deferred_box2d_component_setup(hit_bounds, b2BodyType::b2_dynamicBody, [this](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(k_hit_bounds_size * .5f);
        });
        hit_bounds->add_component(box2d_body_component);
        
        auto hit_bounds_component = std::make_shared<ces_hit_bounds_component>();
        hit_bounds_component->set_executor(executor);
        hit_bounds->add_component(hit_bounds_component);
        
        
        f32 current_rotation = rotation;
        current_rotation += 180.f;
        glm::vec2 current_position = position;
        current_position += glm::vec2(-sinf(glm::radians(current_rotation)) * 64.f,
                                      cosf(glm::radians(current_rotation)) * 64.f);
        
        hit_bounds->position = current_position;
        hit_bounds->rotation = current_rotation;
        
        glm::vec2 velocity = glm::vec2(-sinf(glm::radians(current_rotation)) * std::numeric_limits<i16>::max(),
                                       cosf(glm::radians(current_rotation)) * std::numeric_limits<i16>::max());
        m_hit_bounds.push_back(std::make_tuple(hit_bounds, velocity));
        m_layer.lock()->add_child(hit_bounds);
    }
}
