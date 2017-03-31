//
//  bullet.h
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object.h"

namespace game
{
    class bullet : public gb::game_object
    {
    private:
        
        gb::game_object_weak_ptr m_bullet;
        gb::light_source_weak_ptr m_light_source;
        
    protected:
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        bullet();
        ~bullet();
        
        void setup(const std::string& filename,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                   const gb::ces_entity_shared_ptr& owner);
    };
};
