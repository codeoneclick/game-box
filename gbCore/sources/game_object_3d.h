//
//  game_object_3d.hpp
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_entity.h"

namespace gb
{
    class game_object_3d : public ces_entity
    {
    private:
        
    protected:
        
    public:
        
        game_object_3d();
        virtual ~game_object_3d();
        
        std::property_rw<glm::vec3> position;
        std::property_rw<glm::vec3> rotation;
        std::property_rw<glm::vec3> scale;
    };
};
