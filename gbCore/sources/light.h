//
//  light.h
//  gbCore
//
//  Created by sergey.sergeev on 1/8/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef light_h
#define light_h

#include "renderable_game_object.h"

namespace gb
{
    class light : public renderable_game_object
    {
    private:
        
    protected:
        
        f32 m_radius;
        glm::vec4 m_color;
        
    public:
        
        light();
        ~light();
        
        void set_radius(f32 radius);
        f32 get_radius() const;
        
        void set_color(const glm::vec4& color);
        glm::vec4 get_color() const;
        
        void set_position(const glm::vec2& position);
    };
};

#endif
