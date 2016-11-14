//
//  ces_ani_animation_component.h
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_ani_animation_component_h
#define ces_ani_animation_component_h

#include "ces_base_component.h"

namespace gb
{
    namespace anim
    {
        class ani_timeline;
        class ces_ani_animation_component : public ces_base_component
        {
        private:
            
        protected:
            
            std::shared_ptr<ani_timeline> m_timeline;
            
        public:
            
            CTTI_CLASS_GUID(ces_ani_animation_component, ces_base_component::g_guids_container)
            ces_ani_animation_component();
            ~ces_ani_animation_component();
            
            std::property_rw<std::shared_ptr<ani_timeline>> timeline;
        };
    };
};


#endif 
