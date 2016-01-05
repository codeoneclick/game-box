//
//  ces_box2d_system.h
//  gbCore
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_box2d_system_h
#define ces_box2d_system_h

#include "ces_system.h"

namespace gb
{
    class ces_box2d_system : public ces_system
    {
    private:
        
        collision_manager_shared_ptr m_collision_manager;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        ces_box2d_system();
        ~ces_box2d_system();
        
        collision_manager_shared_ptr get_collision_manager() const;
        void set_box2d_world(const glm::vec2 &min_bound, const glm::vec2 &max_bound);
    };
};

#endif
