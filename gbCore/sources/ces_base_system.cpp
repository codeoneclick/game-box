//
//  ces_base_system.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_base_system.h"

namespace gb
{
    std::set<uintptr_t> ces_base_system::g_guids_container;
    ces_base_system::ces_base_system() :
	m_order(std::numeric_limits<uint8_t>::max())
    {
        m_references_to_required_entities = std::make_shared<std::unordered_map<std::bitset<std::numeric_limits<uint8_t>::max()>, std::list<ces_entity_weak_ptr>>>();
    }
    
    void ces_base_system::init()
    {
        
    }
    
    void ces_base_system::set_current_camera_2d(camera_2d_const_shared_ptr camera)
    {
        m_camera_2d = camera;
    }
    
    camera_2d_shared_ptr ces_base_system::get_current_camera_2d() const
    {
        return m_camera_2d.lock();
    }
    
    void ces_base_system::set_current_camera_3d(camera_3d_const_shared_ptr camera)
    {
        m_camera_3d = camera;
    }
    
    camera_3d_shared_ptr ces_base_system::get_current_camera_3d() const
    {
        return m_camera_3d.lock();
    }

	ui8 ces_base_system::get_order() const
	{
		return m_order;
	}

	void ces_base_system::set_order(ui8 value)
	{
		m_order = value;
	}
}
