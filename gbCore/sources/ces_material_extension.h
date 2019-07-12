//
//  ces_material_extension.hpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_transformation_2d_component.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

namespace gb
{
    class ces_material_extension
    {
    private:
        
    protected:
        
    public:
        
        static void add_material(const ces_entity_shared_ptr& entity,
                                 const std::string& technique_name,
                                 const material_shared_ptr& material);
        static void remove_material(const ces_entity_shared_ptr& entity,
                                    const std::string& technique_name);
        static material_shared_ptr get_material(const ces_entity_shared_ptr& entity,
                                                const std::string& technique_name);
    };
};

#endif
