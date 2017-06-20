//
//  hit_bounds.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 6/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "hit_bounds.h"
#include "ces_box2d_body_component.h"
#include "ces_hit_bounds_component.h"
#include "ces_action_component.h"
#include "ces_sound_component.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "sprite.h"

#define k_hit_bounds_size 16.f

namespace game
{
    
    const std::string hit_bounds::k_create_state = "create";
    const std::string hit_bounds::k_destroy_state = "destroy";
    
    hit_bounds::hit_bounds()
    {
        auto hit_bounds_component = std::make_shared<ces_hit_bounds_component>();
        ces_entity::add_component(hit_bounds_component);
        
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&hit_bounds::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        hit_bounds::add_component(action_component);
    }
    
    hit_bounds::~hit_bounds()
    {
        
    }
    
    void hit_bounds::setup(const std::string& filename,
                             const gb::scene_graph_shared_ptr& scene_graph,
                             const gb::scene_fabricator_shared_ptr& scene_fabricator,
                             const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                             const gb::ces_entity_shared_ptr& owner)
    {
        auto hit_bounds = scene_fabricator->create_sprite(filename);
        hit_bounds->tag = "hit_geometry";
        hit_bounds->pivot = glm::vec2(.5f, .5f);
        hit_bounds->size = glm::vec2(k_hit_bounds_size);
        hit_bounds->color = glm::u8vec4(255, 0, 0, 255);
        hit_bounds::add_child(hit_bounds);
        
        auto hit_bounds_component = hit_bounds::get_component<ces_hit_bounds_component>();
        hit_bounds_component->set_parameters(owner);
    }
    
    void hit_bounds::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {

        
    }
    
    void hit_bounds::attach_sound(const std::string& filename, const std::string& state)
    {
        auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
        if(!sound_component)
        {
            sound_component = std::make_shared<gb::al::ces_sound_component>();
            ces_entity::add_component(sound_component);
        }
        sound_component->add_sound(filename, false);
        m_sounds_linkage.insert(std::make_pair(state, filename));
    }
    
    void hit_bounds::on_create()
    {
        auto sound_linkage = m_sounds_linkage.find(k_create_state);
        auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
        if(sound_component && sound_linkage != m_sounds_linkage.end())
        {
            sound_component->trigger_sound(sound_linkage->second, false);
        }
    }
    
    void hit_bounds::on_destroy()
    {
        auto sound_linkage = m_sounds_linkage.find(k_destroy_state);
        auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
        if(sound_component && sound_linkage != m_sounds_linkage.end())
        {
            sound_component->trigger_sound(sound_linkage->second, false);
        }
    }
}
