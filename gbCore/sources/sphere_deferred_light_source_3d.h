//
//  sphere_deferred_light_source_3d.h
//  gbCore
//
//  Created by serhii.s on 3/22/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "deferred_light_source_3d.h"

namespace gb
{
    class sphere_deferred_light_source_3d : public deferred_light_source_3d
    {
    private:
        
    protected:
        
    public:
        
        sphere_deferred_light_source_3d();
        ~sphere_deferred_light_source_3d();
        
        void setup_components() override;
    };
};

