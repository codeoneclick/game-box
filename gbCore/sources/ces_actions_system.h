//
//  ces_actions_system.h
//  gbCore
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_actions_system_h
#define ces_actions_system_h

#include "ces_system.h"

namespace gb
{
    class ces_actions_system : public ces_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        ces_actions_system();
        ~ces_actions_system();
    };
};

#endif