//
//  ces_geometry_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_geometry_component_h
#define ces_geometry_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_geometry_component : public ces_base_component
    {
    private:
        
    protected:
        
        mesh_shared_ptr m_mesh;
        
    public:
        
        ces_geometry_component();
        ~ces_geometry_component();
        
        mesh_shared_ptr get_mesh() const;
    };
};

#endif
