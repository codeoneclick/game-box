//
//  sprite.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_2d.h"

namespace gb
{
    class sprite : public game_object_2d
    {
    private:
        
    protected:
        
        glm::u8vec4 m_color;
        
    public:
        
        sprite(bool is_use_batch = false);
        virtual ~sprite();
        
        void set_custom_texcoord(const glm::vec4& texcoord);
        
        std::property_rw<glm::vec2> pivot;
        
        std::property_rw<bool> is_shadow_caster;
        std::property_rw<bool> is_luminous;
        
        std::property_rw<glm::u8vec4> color;
        std::property_rw<ui8> alpha;
    };
};
