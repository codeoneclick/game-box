//
//  sprite.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object.h"

namespace gb
{
    class sprite : public game_object
    {
    private:
        
    protected:
        
    public:
        
        sprite();
        virtual ~sprite();
        
        void set_custom_texcoord(const glm::vec4& texcoord);
        
        std::property_rw<glm::vec2> pivot;
        
        std::property_rw<bool> is_shadow_caster;
        std::property_rw<bool> is_luminous;
    };
};
