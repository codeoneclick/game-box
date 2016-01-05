//
//  i_scene_fabricator.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "i_scene_fabricator.h"

namespace gb
{
    i_scene_fabricator::i_scene_fabricator() :
    m_configuration_accessor(nullptr),
    m_resource_accessor(nullptr)
    {
        
    }
    
    void i_scene_fabricator::set_configuration_accessor(const configuration_accessor_shared_ptr& configuration_accessor)
    {
        m_configuration_accessor = configuration_accessor;
    }
    
    void i_scene_fabricator::set_resource_accessor(const resource_accessor_shared_ptr& resource_accessor)
    {
        m_resource_accessor = resource_accessor;
    }
}