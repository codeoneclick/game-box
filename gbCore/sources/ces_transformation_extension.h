//
//  ces_transformation_extension.hpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/10/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_transformation_extension_h
#define ces_transformation_extension_h

#include "ces_transformation_component.h"

namespace gb
{
    class ces_transformation_extension
    {
    private:
        
    protected:
        
    public:
        
        static glm::mat4 get_absolute_matrix(const ces_entity_shared_ptr& entity);
        static glm::mat4 get_absolute_matrix_in_camera_space(const ces_entity_shared_ptr& entity);
    };
};

#endif
