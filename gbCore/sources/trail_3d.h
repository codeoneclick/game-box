//
//  trail.hpp
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"

namespace gb
{
    class trail_3d : public game_object_3d
    {
    private:
        
    protected:
        
    public:
        
        trail_3d();
        ~trail_3d();
        
        void setup_components() override;
    };
};

