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
    private:
        
        gb::shape_3d_weak_ptr m_shape_3d_linkage;
        
    protected:
        
        void setup_light(const gb::scene_graph_shared_ptr& scene_graph,
                         const gb::scene_fabricator_shared_ptr& scene_fabricator,
                         const glm::vec4& ligth_source_color = glm::vec4(1.f));
        
    public:
        
        character();
        ~character();
        
        void setup(const std::string& filename,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                   bool is_enabled_light_source = false,
                   const glm::vec4& ligth_source_color = glm::vec4(1.f));
        
        void setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   bool is_enabled_light_source = false,
                   const glm::vec4& ligth_source_color = glm::vec4(1.f));
        
        void play_animation(const std::string& animation_name);
    };
};
