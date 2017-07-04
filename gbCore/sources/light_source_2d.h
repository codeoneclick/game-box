//
//  light_source_2d.h
//  gbCore
//
//  Created by sergey.sergeev on 1/8/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_2d.h"

namespace gb
{
    class light_source_2d : public game_object_2d
    {
    private:
        
    protected:
        
        f32 m_radius;
        glm::vec4 m_color;
        
    public:
        
        light_source_2d();
        ~light_source_2d();
        
        static light_source_2d_shared_ptr construct();
        
        std::property_rw<f32> radius;
        std::property_rw<glm::vec4> color;
    };
};
