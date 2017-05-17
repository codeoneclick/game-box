//
//  shape_3d.h
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"

namespace gb
{
    class shape_3d : public game_object_3d
    {
    private:
        
    protected:
        
    public:
        
        shape_3d();
        ~shape_3d();
        
        void play_animation(const std::string& animation_name);
    };
};
