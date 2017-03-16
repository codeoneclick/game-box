//
//  ai_character_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object.h"

namespace game
{
    class ai_character_controller : public gb::game_object
    {
    private:
        
    protected:
        
        gb::scene_graph_weak_ptr m_scene_graph;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        gb::anim::anim_fabricator_weak_ptr m_anim_fabricator;
        gb::game_object_weak_ptr m_layer;
        
        gb::game_object_shared_ptr m_character;
        
        void on_shoot();
        
        virtual void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        ai_character_controller(const gb::game_object_shared_ptr& layer,
                                const gb::scene_graph_shared_ptr& scene_graph,
                                const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                const gb::anim::anim_fabricator_shared_ptr& anim_fabricator);
        ~ai_character_controller();
        
        void setup(const std::string& filename);
    };
};

