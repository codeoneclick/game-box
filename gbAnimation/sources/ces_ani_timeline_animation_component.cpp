//
//  ces_ani_timeline_animation_component.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/16/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_ani_timeline_animation_component.h"
#include "ani_timeline.h"

namespace gb
{
    namespace anim
    {
        
        ces_ani_timeline_animation_component::ces_ani_timeline_animation_component() :
        m_timeline(nullptr),
        m_current_frame_index(0),
        m_object_id_reference(-1),
        m_is_cw90(false)
        {
            timeline.setter([=](const std::shared_ptr<ani_timeline>& timeline) {
                m_timeline = timeline;
            });
            timeline.getter([=]() {
                return m_timeline;
            });
            
            current_frame_index.setter([=](i32 current_frame_index) {
                m_current_frame_index = current_frame_index;
            });
            current_frame_index.getter([=]() {
                return m_current_frame_index;
            });
            
            object_id_reference.setter([=](i32 object_id_reference) {
                m_object_id_reference = object_id_reference;
            });
            object_id_reference.getter([=]() {
                return m_object_id_reference;
            });
            
            is_cw90.setter([=](bool is_cw90) {
                m_is_cw90 = is_cw90;
            });
            is_cw90.getter([=]() {
                return m_is_cw90;
            });
        }
        
        ces_ani_timeline_animation_component::~ces_ani_timeline_animation_component()
        {
            
        }
        
        void ces_ani_timeline_animation_component::goto_next_frame()
        {
            m_current_frame_index++;
            if(m_current_frame_index >= m_timeline->get_frames_count())
            {
                m_current_frame_index = 0;
            }
        }
    }
}
