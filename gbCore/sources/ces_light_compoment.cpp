//
//  ces_light_compoment.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_light_compoment.h"

namespace gb
{
    ces_light_compoment::ces_light_compoment()
    {

    }
    
    ces_light_compoment::~ces_light_compoment()
    {
        
    }
    
    void ces_light_compoment::add_shadow_emissive_entity(const ces_entity_shared_ptr& entity)
    {
        m_shadow_emissive_entitites.push_back(entity);
    }
    
    const std::list<ces_entity_shared_ptr>& ces_light_compoment::get_shadow_emissive_entities() const
    {
        return m_shadow_emissive_entitites;
    }
    
    void ces_light_compoment::cleanup()
    {
        m_shadow_emissive_entitites.clear();
    }
};
