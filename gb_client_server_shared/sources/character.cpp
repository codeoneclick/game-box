//
//  character.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "character.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_component.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source.h"

namespace game
{
    character::character()
    {
        character::tag = "character";
    }
    
    character::~character()
    {
        
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
        
#if !defined(__NO_RENDER__)
        
        if(is_enabled_light_source)
        {
            auto light_source = scene_fabricator->create_light_source("light_01.xml");
            light_source->radius = 512.f;
            light_source->color = ligth_source_color;
            light_source->tag = "light_source";
            character::add_child(light_source);
        }
        
#endif
    }
}
