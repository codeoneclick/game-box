//
//  i_scene_fabricator.h
//  gbCore
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef i_scene_fabricator_h
#define i_scene_fabricator_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class i_scene_fabricator
    {
    private:
        
    protected:
        
        configuration_accessor_shared_ptr m_configuration_accessor;
        resource_accessor_shared_ptr m_resource_accessor;
        
    public:
        
        i_scene_fabricator();
        ~i_scene_fabricator() = default;
        
        void set_configuration_accessor(const configuration_accessor_shared_ptr& configuration_accessor);
        void set_resource_accessor(const resource_accessor_shared_ptr& resource_accessor);
    };
};

#endif
