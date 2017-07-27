//
//  hit_bounds_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "ces_entity.h"

namespace game
{
    class hit_bounds_controller : public gb::ces_entity
    {
    private:
        
        static const f32 k_hit_bounds_size;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        gb::ces_entity_weak_ptr m_layer;
        
    protected:
        
        std::vector<std::tuple<gb::game_object_2d_weak_ptr, glm::vec2>> m_hit_bounds;
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 dt);
        
    public:
        
        hit_bounds_controller(const gb::ces_entity_shared_ptr& layer,
                              const gb::scene_fabricator_shared_ptr& scene_fabricator);
        ~hit_bounds_controller();
        
        void setup_components() override;
        
        void push_hit_bounds(const gb::ces_entity_shared_ptr& executor, const glm::vec2& position, f32 rotation);
    };
};

