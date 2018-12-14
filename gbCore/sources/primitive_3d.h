//
//  primitive_3d.hpp
//  gbCore
//
//  Created by serhii.s on 12/12/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"

namespace gb
{
    class primitive_3d : public game_object_3d
    {
    private:
        
    protected:
        
        
        
    public:
        
        primitive_3d();
        ~primitive_3d();
        
        void setup_components() override;
        
        std::property_ro<glm::vec3> min_bound;
        std::property_ro<glm::vec3> max_bound;
    };
};
