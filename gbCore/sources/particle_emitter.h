//
//  particle_emitter.h
//  gbCore
//
//  Created by serhii.s on 3/21/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"

namespace gb
{
    class particle_emitter : public game_object_3d
    {
    private:
        
    protected:
        
        
        
    public:
        
        particle_emitter();
        ~particle_emitter();
        
        void setup_components() override;
    };
};

