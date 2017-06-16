//
//  avatar_icon.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object_2d.h"

namespace game
{
    class avatar_icon : public gb::game_object_2d
    {
    public:
        
    private:
        
        static const std::string k_background_entity_filename;
        static const f32 k_background_size;
        
        gb::shape_3d_weak_ptr m_shape_3d_linkage;
        gb::ces_entity_weak_ptr m_current_avatar;
        
    protected:
        
    public:
        
        avatar_icon();
        ~avatar_icon();
        
        void setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   const gb::anim::anim_fabricator_shared_ptr& anim_fabricator);
        
        void set_avatar(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage);
        void play_animation(const std::string& animation_name, bool is_looped = false);
    };
};
