//
//  ces_base_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_base_component.h"

namespace gb
{
    ces_base_component::ces_base_component() :
    m_type(e_ces_component_type_undefined)
    {
        
    }
    
    i32 ces_base_component::get_type() const
    {
        return m_type;
    }
};