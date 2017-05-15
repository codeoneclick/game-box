//
//  ces_transformation_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_transformation_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_transformation_3d_component.h"

namespace gb
{
    ces_transformation_component::ces_transformation_component()
    {
        
    }
    
    ces_transformation_component::~ces_transformation_component()
    {
        
    }
    
    ces_transformation_2d_component_shared_ptr ces_transformation_component::as_2d()
    {
        return std::static_pointer_cast<ces_transformation_2d_component>(shared_from_this());
    }
    
    ces_transformation_3d_component_shared_ptr ces_transformation_component::as_3d()
    {
        return std::static_pointer_cast<ces_transformation_3d_component>(shared_from_this());
    }
};
