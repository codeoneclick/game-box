//
//  omni_deffered_light_source.hpp
//  gbCore
//
//  Created by serhii.s on 2/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"

namespace gb
{
    class omni_deferred_light_source_3d : public game_object_3d
    {
    private:
        
    protected:
        
    public:
        
        omni_deferred_light_source_3d();
        ~omni_deferred_light_source_3d();
        
        void setup_components() override;
        
        std::property_rw<f32> radius;
        std::property_rw<glm::vec4> color;
    };
};

