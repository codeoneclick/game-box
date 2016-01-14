//
//  sprite.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef sprite_h
#define sprite_h

#include "renderable_game_object.h"

namespace gb
{
    class sprite : public renderable_game_object
    {
    private:
        
    protected:
        
    public:
        
        sprite();
        ~sprite();
        
        void set_size(const glm::vec2& size);
        glm::vec2 get_size() const;
        
        void set_pivot(const glm::vec2& pivot);
        glm::vec2 get_pivot() const;
        
        void set_cast_shadow(bool value);
        bool is_cast_shadow() const;
        
        glm::vec2 get_min_bound() const;
        glm::vec2 get_max_bound() const;
        
        glm::vec4 get_bound_frame() const;
    };
};

#endif
