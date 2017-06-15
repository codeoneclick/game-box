//
//  character.h
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//


#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object_2d.h"

namespace game
{
    class character : public gb::game_object_2d
    {
    public:
        
        struct parts
        {
            static const std::string k_bounds_part;
            static const std::string k_body_part;
            static const std::string k_light_source_part;
        };
        
        struct animations
        {
            static const std::string k_idle_animation;
            static const std::string k_walk_animation;
            static const std::string k_attack_animation;
            static const std::string k_search_animation;
            static const std::string k_die_animation;
        };
        
    private:
        
        gb::shape_3d_weak_ptr m_shape_3d_linkage;
        
        static const std::string k_light_source_entity_filename;
        static const std::string k_bounds_entity_filename;
        
        i32 m_animation_end_callback_id;
        
    protected:
        
        void setup_light(const gb::scene_graph_shared_ptr& scene_graph,
                         const gb::scene_fabricator_shared_ptr& scene_fabricator,
                         const glm::vec4& ligth_source_color = glm::vec4(1.f));
        
    public:
        
        character();
        ~character();
        
        void setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   bool is_enabled_light_source = false,
                   const glm::vec4& ligth_source_color = glm::vec4(1.f));
        
        void play_animation(const std::string& animation_name, bool is_looped = false);
        void set_animation_end_callback(const std::function<void(const std::string&, bool)> callback);
    };
};
