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
        
        std::property_rw<glm::vec2> position;
        std::property_rw<f32> rotation;
        std::property_rw<glm::vec2> scale;
        
        std::property_rw<glm::vec2> size;
        std::property_ro<glm::vec4> bound;
    };
};

#endif
