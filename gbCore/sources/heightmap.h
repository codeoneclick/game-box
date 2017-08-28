//
//  heightmap.h
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"

namespace gb
{
    class heightmap : public game_object_3d
    {
    private:
        
    protected:
        
        
        
    public:
        
        heightmap();
        ~heightmap();
        
        void setup_components() override;
    };
};
