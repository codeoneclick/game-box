//
//  omni_deffered_light_source.hpp
//  gbCore
//
//  Created by serhii.s on 2/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"
#include "ces_shader_uniforms_component.h"

namespace gb
{
    class deferred_light_source_3d : public game_object_3d
    {
    private:
        
    protected:
        
    public:
        
        deferred_light_source_3d();
        virtual ~deferred_light_source_3d() = default;
        
        std::property_rw<glm::vec4> color;
    };
};

