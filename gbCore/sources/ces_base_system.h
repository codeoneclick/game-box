//
//  ces_base_system.h
//  gbCore
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_system_h
#define ces_system_h

#include "main_headers.h"
#include "ces_entity.h"


namespace gb
{
    class ces_base_system
    {
    private:
        
        friend class ces_systems_feeder;
        
    protected:
        
        i32 m_priority;
        
        static std::set<uintptr_t> g_guids_container;
        
        virtual void on_feed_start(f32 deltatime) = 0;
        virtual void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime) = 0;
        virtual void on_feed_end(f32 deltatime) = 0;
        
        ces_base_system();
        
    public:
        
        CTTI_CLASS_GUID(ces_base_system, ces_base_system::g_guids_container)
        virtual ~ces_base_system() = default;
        
        i32 get_priority() const;
    };
};


#endif
