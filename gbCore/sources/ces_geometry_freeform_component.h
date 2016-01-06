//
//  ces_geometry_freeform_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_geometry_freeform_component_h
#define ces_geometry_freeform_component_h

#include "ces_geometry_component.h"

namespace gb
{
    class ces_geometry_freeform_component : public ces_geometry_component
    {
    private:
        
    protected:
        
    public:
        
        ces_geometry_freeform_component();
        ~ces_geometry_freeform_component();
        
        void set_mesh(const mesh_shared_ptr& mesh);
    };
};

#endif 
