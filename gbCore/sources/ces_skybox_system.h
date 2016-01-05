//
//  ces_skybox_system.h
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_skybox_system_h
#define ces_skybox_system_h

#include "ces_system.h"

namespace gb
{
    class ces_skybox_system : public ces_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        ces_skybox_system();
        ~ces_skybox_system();
    };
};


#endif
