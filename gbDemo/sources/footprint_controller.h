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
        
        gb::scene_graph_weak_ptr m_scene_graph;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        gb::game_object_2d_weak_ptr m_layer;
        
    protected:
        
        std::vector<game::footprint_weak_ptr> m_footprints;
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        footprint_controller(const gb::game_object_2d_shared_ptr& layer,
                             const gb::scene_graph_shared_ptr& scene_graph,
                             const gb::scene_fabricator_shared_ptr& scene_fabricator);
        ~footprint_controller();
        
        void setup_components() override;
        
        void push_footprint(const glm::u8vec4& color, const glm::vec2& position, f32 rotation);
        
        const std::vector<game::footprint_weak_ptr>& get_footprints() const;
    };
};

