//
//  footprint_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "ces_entity.h"

namespace game
{
    class footprint_controller : public gb::ces_entity
    {
    private:
        
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        gb::ces_entity_weak_ptr m_layer;
        std::chrono::steady_clock::time_point m_previous_timestamp;
        static const f32 k_timeinterval;
        
    protected:
        
        std::vector<game::footprint_weak_ptr> m_footprints;
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        footprint_controller(const gb::ces_entity_shared_ptr& layer,
                             const gb::scene_fabricator_shared_ptr& scene_fabricator);
        ~footprint_controller();
        
        void setup_components() override;
        
        bool is_ready_to_push_footprint() const;
        void push_footprint(const glm::u8vec4& color, const glm::vec2& position, f32 rotation);
        
        const std::vector<game::footprint_weak_ptr>& get_footprints() const;
    };
};

