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
    ces_base_system::ces_base_system()
    {
        
    }
    
    void ces_base_system::init()
    {
        
    }
    
    void ces_base_system::set_current_camera(camera_const_shared_ptr camera)
    {
        m_camera = camera;
    }
    
    camera_shared_ptr ces_base_system::get_current_camera() const
    {
        return m_camera.lock();
    }
}
