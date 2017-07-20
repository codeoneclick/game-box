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
#include "declarations.h"

namespace gb
{
    class ces_systems_feeder : public i_game_loop
    {
    private:
        
        friend class ces_entity;
        
        enum e_entity_state
        {
            e_entity_state_added = 0,
            e_entity_state_removed,
            e_entity_state_changed
        };
        
    protected:
        
        std::map<i32, ces_base_system_shared_ptr> m_systems;
        std::list<ces_base_system_shared_ptr> m_ordered_systems;
        ces_entity_shared_ptr m_root;
        
        std::queue<std::pair<e_entity_state, ces_entity_weak_ptr>> m_changed_entities;
        
        void on_update(f32 deltatime);
        
        void on_entity_added(const ces_entity_shared_ptr& entity);
        void on_entity_removed(const ces_entity_shared_ptr& entity);
        
        void on_entity_component_added(const ces_entity_shared_ptr& entity);
        void on_entity_component_removed(const ces_entity_shared_ptr& entity);
        
    public:
        
        ces_systems_feeder();
        ~ces_systems_feeder();
        
        void add_system(const ces_base_system_shared_ptr& system);
        void remove_system(const i32 system_guid);
        
        void set_root(const ces_entity_shared_ptr& root);
        
        void cleanup();
        
        ces_base_system_shared_ptr get_system(i32 guid) const;
        template<class TSystem> std::shared_ptr<TSystem> get_system() const
        {
            std::shared_ptr<TSystem> system = std::static_pointer_cast<TSystem>(ces_systems_feeder::get_system(TSystem::class_guid()));
            return system;
        }
    };
};

#endif
