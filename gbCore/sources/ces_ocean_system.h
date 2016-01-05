//
//  ces_ocean_system.h
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_ocean_system_h
#define ces_ocean_system_h

#include "ces_system.h"

namespace gb
{
    class ces_ocean_system : public ces_system
    {
    private:
        
        resource_accessor_shared_ptr m_resource_accessor;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        ces_ocean_system(const resource_accessor_shared_ptr& resource_accessor);
        ~ces_ocean_system();
    };
};

#endif
