//
//  ces_ani_main_timeline_animation_component.h
//  gbAnimation
//
//  Created by serhii serhiiv on 11/16/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_ani_main_timeline_animation_component_h
#define ces_ani_main_timeline_animation_component_h

#include "ces_base_component.h"
#include "tags_defines.h"

namespace gb
{
    namespace anim
    {
        class ani_timeline;
        class ces_ani_main_timeline_animation_component : public ces_base_component
        {
        private:
            
        protected:
            
            i32 m_current_frame_index;
            std::shared_ptr<ani_timeline> m_timeline;
            i32 m_object_id_reference;
            bool m_is_cw90;
            
        public:
            
            CTTI_CLASS_GUID(ces_ani_main_timeline_animation_component, ces_base_component::g_guids_container)
            ces_ani_main_timeline_animation_component();
            ~ces_ani_main_timeline_animation_component();
            
            std::property_rw<std::shared_ptr<ani_timeline>> timeline;
            std::property_rw<i32> current_frame_index;
            std::property_rw<i32> object_id_reference;
            std::property_rw<bool> is_cw90;
            
            void goto_next_frame();
        };
    };
};

#endif
