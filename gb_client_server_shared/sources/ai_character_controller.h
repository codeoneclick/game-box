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
#include "game_object_2d.h"
#include "level.h"
#include "client_base_character_controller.h"

namespace game
{
    class ai_character_controller : public client_base_character_controller
    {
    private:
        
    protected:
        
        void on_shoot();
        void on_dead(const gb::ces_entity_shared_ptr& entity) override;
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime) override;
        
    public:
        
        ai_character_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                const std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max>& layers);
        ~ai_character_controller();
    };
};

