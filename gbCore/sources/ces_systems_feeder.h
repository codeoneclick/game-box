//
//  systems_feeder.h
//  gbCore
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef systems_feeder_h
#define systems_feeder_h

#include "ces_systems_feeder.h"
#include "game_loop.h"
#include "ces_system_types.h"
#include "declarations.h"

namespace gb
{
    class ces_systems_feeder : public i_game_loop
    {
    private:
        
    protected:
        
        std::map<ces_system_type, ces_system_shared_ptr> m_systems;
        std::list<ces_system_shared_ptr> m_ordered_systems;
        ces_entity_shared_ptr m_root;
        
        void on_update(f32 deltatime);
        
    public:
        
        ces_systems_feeder();
        ~ces_systems_feeder();
        
        void add_system(const ces_system_shared_ptr& system);
        void remove_system(const ces_system_type type);
        
        void set_root(const ces_entity_shared_ptr& entity);
        
        ces_system_shared_ptr get_system(ces_system_type type) const;
    };
};

#endif
