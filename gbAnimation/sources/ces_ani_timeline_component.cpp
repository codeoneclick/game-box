//
//  ces_ani_timeline_component.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_ani_timeline_component.h"
#include "ani_timeline.h"

namespace gb
{
    namespace anim
    {
        ces_ani_timeline_component::ces_ani_timeline_component() :
        m_timeline(nullptr),
        m_current_frame_index(0),
        m_object_id_reference(-1),
        m_is_playing(false),
        m_is_lopped(false),
        m_current_playing_sequence_name(""),
        m_begin_frame(0),
        m_end_frame(0)
        {
            timeline.setter([=](ani_timeline_shared_ptr timeline) {
                m_timeline = timeline;
                m_begin_frame = 0;
                m_end_frame = m_timeline->get_frames_count();
            });
            timeline.getter([=]() {
                return m_timeline;
            });
            
            current_frame_index.setter([=](i32 current_frame_index) {
                if(m_timeline && current_frame_index < m_timeline->get_frames_count())
                {
                    m_current_frame_index = current_frame_index;
                }
                else
                {
                    assert(false);
                }
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
            
            is_playing.setter([=](bool value) {
                m_is_playing = value;
            });
            is_playing.getter([=]() {
                return m_is_playing;
            });
            
            is_looped.setter([=](bool value) {
                m_is_lopped = value;
            });
            is_looped.getter([=]() {
                return m_is_lopped;
            });
            
            frame_iteration.setter([=](ui64 value) {
                m_frame_iteration = value;
            });
            frame_iteration.getter([=]() {
                return m_frame_iteration;
            });
            
            current_playing_sequence_name.setter([=](const std::string value) {
                m_current_playing_sequence_name = value;
            });
            current_playing_sequence_name.getter([=]() {
                return m_current_playing_sequence_name;
            });
        }
        
        ces_ani_timeline_component::~ces_ani_timeline_component()
        {
            
        }
        
        void ces_ani_timeline_component::set_sequence_interval(i32 begin, i32 end)
        {
            m_begin_frame = begin;
            m_end_frame = end;
        }
        
        void ces_ani_timeline_component::next_frame(f32 dt)
        {
            if(m_is_playing)
            {
                m_current_frame_index++;
                if(m_current_frame_index >= m_end_frame)
                {
                    m_current_frame_index = m_begin_frame;
                }
            }
        }
    }
}

