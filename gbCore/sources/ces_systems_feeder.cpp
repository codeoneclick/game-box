//
//  ces_systems_feeder.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_systems_feeder.h"
#include "ces_base_system.h"

namespace gb
{
    ces_systems_feeder::ces_systems_feeder()
    {
        
    }
    
    ces_systems_feeder::~ces_systems_feeder()
    {
        
    }
    
    ces_base_system_shared_ptr ces_systems_feeder::get_system(i32 guid) const
    {
        const auto& system = m_systems.find(guid);
        if(system != m_systems.end())
        {
            return system->second;
        }
        return nullptr;
    }
    
    void ces_systems_feeder::on_update(f32 deltatime)
    {
        if(m_root)
        {
            for(const auto& system : m_ordered_systems)
            {
                system->on_feed_start(deltatime);
            }
            
            for(const auto& system : m_ordered_systems)
            {
                system->on_feed(m_root, deltatime);
            }
            
            for(const auto& system : m_ordered_systems)
            {
                system->on_feed_end(deltatime);
            }
        }
    }
    
    void ces_systems_feeder::add_system(const ces_base_system_shared_ptr& system)
    {
        m_systems[system->instance_guid()] = system;
        m_ordered_systems.push_back(system);
    }
    
    void ces_systems_feeder::remove_system(i32 type)
    {
        const auto& system = m_systems.find(type);
        if(system != m_systems.end())
        {
            m_systems.erase(system);
            const auto& iterator = std::find(m_ordered_systems.begin(), m_ordered_systems.end(), system->second);
            if(iterator != m_ordered_systems.end())
            {
                m_ordered_systems.erase(iterator);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    void ces_systems_feeder::set_root(const ces_entity_shared_ptr &entity)
    {
        m_root = entity;
    }
    
    void ces_systems_feeder::cleanup()
    {
        m_systems.clear();
        m_ordered_systems.clear();
        ces_systems_feeder::set_root(nullptr);
    }
}
