//
//  ces_ani_main_timeline_animation_component.h
//  gbAnimation
//
//  Created by serhii serhiiv on 11/16/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_ani_base_timeline_component.h"

namespace gb
{
    namespace anim
    {
        class ces_ani_main_timeline_animation_component : public ces_ani_base_timeline_component
        {
        private:
            
        protected:
            
        public:
            
            CTTI_CLASS_GUID(ces_ani_main_timeline_animation_component, ces_base_component::g_guids_container)
            ces_ani_main_timeline_animation_component();
            ~ces_ani_main_timeline_animation_component();
        };
    };
};
