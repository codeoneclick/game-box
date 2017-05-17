//
//  ces_animation_3d_mixer_component.h
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

#define k_blending_animation_timeinterval .25f
#define k_blending_animation_interpolation_multiplier 1.f / k_blending_animation_timeinterval

namespace gb
{
    class ces_animation_3d_mixer_component : public ces_base_component
    {
    private:
        
    protected:
        
        f32 m_animation_time;
        f32 m_blending_animation_timeinterval;
        std::string m_current_animation_name;
        
        ui32 m_current_animation_frame;
        ui32 m_blending_animation_frame;
        
        animation_3d_sequence_shared_ptr m_previous_animation_sequence;
        animation_3d_sequence_shared_ptr m_current_animation_sequence;
        
        std::map<std::string, animation_3d_sequence_shared_ptr> m_animations_sequences;
        glm::mat4* m_bones_transformations;
        bool m_is_binded;

    public:
        
        CTTI_CLASS_GUID(ces_animation_3d_mixer_component, ces_base_component::g_guids_container)
        ces_animation_3d_mixer_component();
        ~ces_animation_3d_mixer_component();
        
        void setup(const skeleton_3d_transfering_data_shared_ptr& skeleton_transfering_data,
                   const sequence_3d_transfering_data_shared_ptr& sequence_transfering_data);
        
        void add_animation_sequence(const animation_3d_sequence_shared_ptr& sequence);
        void remove_animation_sequence(const animation_3d_sequence_shared_ptr& sequence);
        
        void set_animation(const std::string& name);
        bool is_animated();
        
        void set_pose_binded(bool value);
        bool get_pose_binded() const;
        
        bool try_bind_animation_sequence(const std::string& animation_name);
        
        std::string get_current_animation_name() const;
        
        void update_animation_time(f32 dt);
        f32 get_animation_time() const;
        
        void update_blending_animation_timeinterval(f32 dt);
        f32 get_blending_animation_timeinterval() const;
        
        void reset_animation_time();
        void reset_previous_animation_sequence();
        
        ui32 get_current_animation_frame() const;
        ui32 get_blending_animation_frame() const;
        
        void update_curret_animation_frame(ui32 current_frame);
        
        animation_3d_sequence_shared_ptr get_current_animation_sequence() const;
        animation_3d_sequence_shared_ptr get_previous_animation_sequence() const;
    };
};
