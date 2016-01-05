//
//  ces_system.h
//  gbCore
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_system_h
#define ces_system_h

#include "main_headers.h"
#include "ces_system_types.h"
#include "ces_entity.h"

class ces_entity;

namespace gb
{
    class ces_system
    {
    private:
        
        friend class ces_systems_feeder;
        
    protected:
        
        i32 m_type;
        i32 m_priority;
        
        virtual void on_feed_start(f32 deltatime) = 0;
        virtual void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime) = 0;
        virtual void on_feed_end(f32 deltatime) = 0;
        
        ces_system();
        
    public:
        
        virtual ~ces_system() = default;
        
        i32 get_type() const;
        i32 get_priority() const;
    };
};


#endif
