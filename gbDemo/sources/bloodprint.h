//
//  bloodprint.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object.h"

namespace game
{
    class bloodprint : public gb::game_object
    {
    private:
        
        gb::sprite_weak_ptr m_bloodprint;
        glm::vec2 m_size;
        
    protected:
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
        f32 m_max_visible_time;
        f32 m_visible_time;
        
    public:
        
        bloodprint();
        ~bloodprint();
        
        void setup(const std::string& filename,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   const gb::anim::anim_fabricator_shared_ptr& anim_fabricator);
        
        std::property_ro<bool> is_expired;
    };
};

