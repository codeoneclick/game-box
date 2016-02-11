//
//  light_source.h
//  gbCore
//
//  Created by sergey.sergeev on 1/8/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef light_source_h
#define light_source_h

#include "game_object.h"

namespace gb
{
    class light_source : public game_object
    {
    private:
        
    protected:
        
        f32 m_radius;
        glm::vec4 m_color;
        
    public:
        
        light_source();
        ~light_source();
        
        std::property_rw<f32> radius;
        std::property_rw<glm::vec4> color;
    };
};

#endif
