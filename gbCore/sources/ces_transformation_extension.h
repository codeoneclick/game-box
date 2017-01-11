//
//  ces_transformation_extension.hpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/10/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_transformation_component.h"

namespace gb
{
    class ces_transformation_extension
    {
    private:
        
    protected:
        
    public:
        
        static glm::mat4 get_absolute_transformation_in_ws(const ces_entity_shared_ptr& entity);
        static glm::mat4 get_absolute_transformation_in_ss(const ces_entity_shared_ptr& entity, const camera_shared_ptr& camera);
        
        static void update_absolute_transformation_recursively(const ces_entity_shared_ptr& entity);
    };
};
