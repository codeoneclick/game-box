//
//  ces_ani_timeline_component.h
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "tags_defines.h"
#include "ani_declarations.h"

namespace gb
{
    namespace anim
    {
        class ces_ani_timeline_component : public ces_base_component
        {
        private:
            
        protected:
            
            ani_timeline_shared_ptr m_timeline;
            i32 m_current_frame_index;
            i32 m_object_id_reference;
            
            bool m_is_playing;
            bool m_is_lopped;
            
            ui64 m_frame_iteration;
            
        public:
            
            CTTI_CLASS_GUID(ces_ani_timeline_component, ces_base_component::g_guids_container)
            ces_ani_timeline_component();
            ~ces_ani_timeline_component();
            
            std::property_rw<ani_timeline_shared_ptr> timeline;
            std::property_rw<i32> current_frame_index;
            std::property_rw<i32> object_id_reference;
            
            std::property_rw<bool> is_looped;
            std::property_rw<bool> is_playing;
            
            std::property_rw<ui64> frame_iteration;
            
            void next_frame(f32 dt);
        };
    };
};
