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
    class trail : public game_object_3d
    {
    private:
        
    protected:
        
    public:
        
        trail();
        ~trail();
        
        void setup_components() override;
    };
};

