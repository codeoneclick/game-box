//
//  game_object.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef game_object_h
#define game_object_h

#include "ces_entity.h"

namespace gb
{
    class game_object : public ces_entity
    {
    private:
        
    protected:
        
    public:
        
        game_object();
        virtual ~game_object();
        
        virtual void set_position(const glm::vec2& position);
        glm::vec2 get_position() const;
        
        void set_rotation(f32 rotation);
        f32 get_rotation() const;
        
        void set_scale(const glm::vec2& scale);
        glm::vec2 get_scale() const;
        
        virtual glm::vec4 get_bound() const;
        
        glm::mat4 get_ws_mat_m() const;
        glm::mat4 get_cs_mat_m() const;
    };
};

#endif
