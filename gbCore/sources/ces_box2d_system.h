//
//  ces_box2d_system.h
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_box2d_system_h
#define ces_box2d_system_h

#include "ces_base_system.h"

namespace gb
{
    class ces_box2d_system : public ces_base_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        CTTI_CLASS_GUID(ces_box2d_system, ces_base_system::g_guids_container)
        ces_box2d_system();
        ~ces_box2d_system();
    };
};

#endif
