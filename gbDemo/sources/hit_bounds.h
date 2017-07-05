//
//  hit_bounds.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object_2d.h"

namespace game
{
    class hit_bounds : public gb::game_object_2d
    {
    private:
        
        std::unordered_map<std::string, std::string> m_sounds_linkage;
        
    protected:
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 dt);
        
    public:
        
        hit_bounds();
        ~hit_bounds();
        
        void setup_components() override;
        
        static const std::string k_create_state;
        static const std::string k_destroy_state;
        
        void setup(const std::string& filename,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                   const gb::ces_entity_shared_ptr& owner);
        
        void attach_sound(const std::string& filename, const std::string& state);
        
        void on_create();
        void on_destroy();
    };
};
