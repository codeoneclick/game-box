//
//  ces_material_extension.cpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_material_extension.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "ces_material_component.h"
#include "ces_entity.h"

namespace gb
{
        
    void ces_material_extension::add_material(const ces_entity_shared_ptr& entity,
                                              const std::string& technique_name,
                                              const material_shared_ptr& material)
    {
        entity->get_component<ces_material_component>()->add_material(technique_name, material);
    }
    
    void ces_material_extension::remove_material(const ces_entity_shared_ptr& entity,
                                                 const std::string& technique_name)
    {
        entity->get_component<ces_material_component>()->remove_material(technique_name);
    }
    
    material_shared_ptr ces_material_extension::get_material(const ces_entity_shared_ptr& entity,
                                                             const std::string& technique_name)
    {
        return entity->get_component<ces_material_component>()->get_material(technique_name);
    }
};

#endif
