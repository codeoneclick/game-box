//
//  ces_particle_emitter_system.h
//  gbCore
//
//  Created by sergey.sergeev on 9/2/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_particle_emitter_system_h
#define ces_particle_emitter_system_h

#include "ces_system.h"

namespace gb
{
    class ces_particle_emitter_system : public ces_system
    {
    private:
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        ces_particle_emitter_system();
        ~ces_particle_emitter_system();
    };
};

#endif
