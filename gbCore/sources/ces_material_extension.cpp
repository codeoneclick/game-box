//
//  ces_material_extension.cpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_material_extension.h"
#include "ces_material_component.h"
#include "ces_entity.h"

namespace gb
{
        
    void ces_material_extension::add_material(const ces_entity_shared_ptr& entity,
                                              const std::string& technique_name, i32 technique_pass,
                                              const material_shared_ptr& material)
    {
        unsafe_get_material_component(entity)->add_material(technique_name, technique_pass, material);
    }
    
    void ces_material_extension::remove_material(const ces_entity_shared_ptr& entity,
                                                 const std::string& technique_name, i32 technique_pass)
    {
        unsafe_get_material_component(entity)->remove_material(technique_name, technique_pass);
    }
    
    material_shared_ptr ces_material_extension::get_material(const ces_entity_shared_ptr& entity,
                                                             const std::string& technique_name, i32 technique_pass)
    {
        return unsafe_get_material_component(entity)->get_material(technique_name, technique_pass);
    }
};
