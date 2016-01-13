//
//  control.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "control.h"

namespace gb
{
    namespace ui
    {
        control::control(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator)
        {
            
        }
        
        control::~control()
        {
            
        }
        
        scene_fabricator_shared_ptr control::get_fabricator() const
        {
            return m_fabricator.lock();
        }
    }
}