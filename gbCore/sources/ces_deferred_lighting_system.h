//
//  ces_deferred_lighting_system.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_deferred_lighting_system_h
#define ces_deferred_lighting_system_h

#include "ces_system.h"

namespace gb
{
    class ces_deferred_lighting_system : public ces_system
    {
    private:
        
    protected:
        
        std::set<ces_entity_shared_ptr> m_light_casters;
        std::set<ces_entity_shared_ptr> m_shadow_casters;
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const ces_entity_shared_ptr& entity);
        
    public:
        
        ces_deferred_lighting_system();
        ~ces_deferred_lighting_system();
    };
};


#endif
