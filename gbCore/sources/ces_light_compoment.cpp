//
//  ces_light_compoment.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "ces_light_compoment.h"

namespace gb
{
    ces_light_compoment::ces_light_compoment()
    {

    }
    
    ces_light_compoment::~ces_light_compoment()
    {
        ces_light_compoment::cleanup();
    }
    
    void ces_light_compoment::add_luminous_entity(const ces_entity_shared_ptr& entity)
    {
        m_luminous_entities.push_back(entity);
    }
    
    const std::list<ces_entity_weak_ptr>& ces_light_compoment::get_luminous_entities() const
    {
        return m_luminous_entities;
    }
    
    void ces_light_compoment::cleanup()
    {
        m_luminous_entities.clear();
    }
};

#endif
