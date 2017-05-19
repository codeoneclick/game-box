//
//  character.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "character.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_2d_component.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source_2d.h"
#include "shape_3d.h"

namespace game
{
    character::character()
    {
        character::tag = "character";
    }
    
    character::~character()
    {
        
    }
    
    void character::setup_light(const gb::scene_graph_shared_ptr& scene_graph,
                                const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                const glm::vec4& ligth_source_color)
    {
        auto light_source = scene_fabricator->create_light_source_2d("light_01.xml");
        light_source->radius = 512.f;
        light_source->color = ligth_source_color;
        light_source->tag = "light_source";
        character::add_child(light_source);
    }
    
    void character::setup(const std::string& filename,
                          const gb::scene_graph_shared_ptr& scene_graph,
                          const gb::scene_fabricator_shared_ptr& scene_fabricator,
                          const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                          bool is_enabled_light_source, const glm::vec4& ligth_source_color)
    {
        auto feet = anim_fabricator->create_animated_sprite(filename, "feet_animation");
        feet->tag = "feet";
        feet->goto_and_play("idle");
        feet->is_luminous = true;
        feet->rotation = -90.f;
        character::add_child(feet);
        
        auto body = anim_fabricator->create_animated_sprite(filename, "character_animation_shotgun");
        body->tag = "body";
        body->goto_and_play("idle");
        body->is_luminous = true;
        body->rotation = -90.f;
        character::add_child(body);
        
        if(is_enabled_light_source)
        {
            character::setup_light(scene_graph, scene_fabricator, ligth_source_color);
        }
    }
    
    void character::setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage,
                          const gb::scene_graph_shared_ptr& scene_graph,
                          const gb::scene_fabricator_shared_ptr& scene_fabricator,
                          bool is_enabled_light_source,
                          const glm::vec4& ligth_source_color)
    {
        m_shape_3d_linkage = character_linkage.second;
        character::add_child(character_linkage.first);
        if(is_enabled_light_source)
        {
            character::setup_light(scene_graph, scene_fabricator, ligth_source_color);
        }
    }
    
    void character::play_animation(const std::string &animation_name)
    {
        std::list<gb::ces_entity_shared_ptr> children = character::children;
        for(const auto& child : children)
        {
            std::string part_name = child->tag;
            if(part_name == "feet" || part_name == "body")
            {
                gb::anim::animated_sprite_shared_ptr part = std::static_pointer_cast<gb::anim::animated_sprite>(child);
                part->goto_and_play(animation_name);
            }
        }
        if(!m_shape_3d_linkage.expired())
        {
            m_shape_3d_linkage.lock()->play_animation(animation_name);
        }
    }
}

