//
//  trail_controller.hpp
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_entity.h"

namespace gb
{
    class trail_controller : public ces_entity
    {
    private:
        
        gb::ces_entity_weak_ptr m_layer;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        
    protected:
        
        std::vector<trail_weak_ptr> m_trails;
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        trail_controller(const gb::ces_entity_shared_ptr& layer,
                         const gb::scene_fabricator_shared_ptr& scene_fabricator);
        ~trail_controller();
        
        void setup_components() override;
        
        void push_trail(const std::string& filename,
                        const glm::vec3& start_position);
    };
};
