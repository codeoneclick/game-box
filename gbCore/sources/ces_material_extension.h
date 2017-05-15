//
//  ces_material_extension.hpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "ces_transformation_2d_component.h"

namespace gb
{
    class ces_material_extension
    {
    private:
        
    protected:
        
    public:
        
        static void add_material(const ces_entity_shared_ptr& entity,
                                 const std::string& technique_name, i32 technique_pass,
                                 const material_shared_ptr& material);
        static void remove_material(const ces_entity_shared_ptr& entity,
                                    const std::string& technique_name, i32 technique_pass);
        static material_shared_ptr get_material(const ces_entity_shared_ptr& entity,
                                                const std::string& technique_name, i32 technique_pass);
    };
};

#endif
