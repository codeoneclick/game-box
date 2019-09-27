//
//  ces_camera_collision_component.hpp
//  gbCore
//
//  Created by serhii.s on 7/24/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_camera_collision_component : public ces_base_component
    {
        private:
        

        protected:
        
        public:
        
        STTI_CLASS_GUID(ces_camera_collision_component, ces_base_component::g_guids_container)
        ces_camera_collision_component() = default;
        ~ces_camera_collision_component() = default;
    };
};
