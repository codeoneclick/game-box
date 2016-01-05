//
//  ces_camera_system.h
//  gbCore
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_camera_system_h
#define ces_camera_system_h

#include "ces_system.h"

namespace gb
{
    class ces_camera_system : public ces_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        ces_camera_system();
        ~ces_camera_system();
    };
};

#endif
