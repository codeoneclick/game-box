//
//  ces_geometry_extension.h
//  gbCore
//
//  Created by Serhii Serhiiv on 2/16/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_geometry_component.h"

namespace gb
{
    class ces_geometry_extension
    {
    private:
        
    protected:
        
    public:
        
        static glm::vec4 get_absolute_position_bounds(const ces_entity_shared_ptr& entity);
    };
};
