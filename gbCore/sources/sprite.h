//
//  sprite.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef sprite_h
#define sprite_h

#include "game_object.h"

namespace gb
{
    class sprite : public game_object
    {
    private:
        
    protected:
        
    public:
        
        sprite();
        ~sprite();
        
        void set_custom_texcoord(const glm::vec4& texcoord);
        
        std::property_rw<glm::vec2> pivot;
        
        std::property_rw<bool> cast_shadow;
        std::property_rw<bool> ignore_shadow;
    };
};

#endif
