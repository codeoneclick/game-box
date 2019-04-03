//
//  ces_character_parts_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_parts_component.h"

namespace game
{
    const std::string ces_character_parts_component::parts::k_body = "body";
    const std::string ces_character_parts_component::parts::k_bounds_part = "bounds";
    const std::string ces_character_parts_component::parts::k_light_source_part = "light_source";
    const std::string ces_character_parts_component::parts::k_fl_tire = "fl_tire";
    const std::string ces_character_parts_component::parts::k_fr_tire = "fr_tire";
    const std::string ces_character_parts_component::parts::k_rl_tire = "rl_tire";
    const std::string ces_character_parts_component::parts::k_rr_tire = "rr_tire";
    const std::string ces_character_parts_component::parts::k_fl_light = "fl_light";
    const std::string ces_character_parts_component::parts::k_fr_light = "fr_light";
    const std::string ces_character_parts_component::parts::k_rl_tire_particles = "rl_tire_particles";
    const std::string ces_character_parts_component::parts::k_rr_tire_particles = "rr_tire_particles";
    
    ces_character_parts_component::ces_character_parts_component()
    {
        
    }
    
    ces_character_parts_component::~ces_character_parts_component()
    {
        
    }
    
    void ces_character_parts_component::setup(const gb::ces_entity_shared_ptr& body_part,
                                              const gb::ces_entity_shared_ptr& bounds_part,
                                              const gb::ces_entity_shared_ptr& ligth_source_part)
    {
        m_body_part = body_part;
        m_bounds_part = bounds_part;
        m_light_source_part = ligth_source_part;
    }
    
    gb::ces_entity_shared_ptr ces_character_parts_component::get_bounds_part() const
    {
        return m_bounds_part.lock();
    }
    
    gb::ces_entity_shared_ptr ces_character_parts_component::get_light_source_part() const
    {
        return m_light_source_part.lock();
    }
    
    gb::ces_entity_shared_ptr ces_character_parts_component::get_body_part() const
    {
        return m_body_part.lock();
    }
    
    void ces_character_parts_component::add_part(const gb::ces_entity_shared_ptr& part, const std::string& part_name)
    {
        m_parts[part_name] = part;
    }
    
    gb::ces_entity_shared_ptr ces_character_parts_component::get_part(const std::string& part_name) const
    {
        gb::ces_entity_shared_ptr part = nullptr;
        auto part_it = m_parts.find(part_name);
        if (part_it != m_parts.end())
        {
            part = part_it->second.lock();
        }
        return part;
    }
}
