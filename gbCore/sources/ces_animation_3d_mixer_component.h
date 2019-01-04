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
    public:
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&, const std::string&, bool)> on_animation_ended_callback_t;
        
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
        i32 m_num_bones_transformations;
        bool m_is_binded;
        
        bool m_is_looped;
        bool m_is_animation_ended;
        
        i32 m_custom_animation_fps;
        
        std::vector<std::tuple<gb::ces_entity_weak_ptr, on_animation_ended_callback_t>> m_on_animation_ended_callbacks;
        
        i32 m_previous_played_frame;

    public:
        
        CTTI_CLASS_GUID(ces_animation_3d_mixer_component, ces_base_component::g_guids_container)
        ces_animation_3d_mixer_component();
        ~ces_animation_3d_mixer_component();
        
        void setup(const skeleton_3d_transfering_data_shared_ptr& skeleton_transfering_data,
                   const sequence_3d_transfering_data_shared_ptr& sequence_transfering_data);
        
        void add_animation_sequence(const animation_3d_sequence_shared_ptr& sequence);
        void remove_animation_sequence(const animation_3d_sequence_shared_ptr& sequence);
        
        void set_animation(const std::string& name, bool is_looped = false);
        bool is_animated();
        
        void set_pose_binded(bool value);
        bool get_pose_binded() const;
        
        bool validate_current_animation_sequence();
        
        std::string get_current_animation_name() const;
        
        bool get_is_looped() const;
        
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
        
        glm::mat4* get_transformations() const;
        i32 get_transformation_size() const;
        
        bool is_animation_ended_callback_exist(const gb::ces_entity_shared_ptr& owner);
        void add_animation_ended_callback(const ces_entity_shared_ptr& owner, const on_animation_ended_callback_t& callback);
        void remove_animation_ended_callback(const ces_entity_shared_ptr& owner);
        const std::vector<std::tuple<gb::ces_entity_weak_ptr, on_animation_ended_callback_t>>& get_animation_ended_callbacks() const;
        
        void interrupt_animation();
        bool get_is_animation_ended() const;
        
        void set_custom_animation_fps(i32 fps);
        i32 get_cutom_animation_fps() const;
        
        void set_previous_played_frame(i32 frame);
        i32 get_previous_played_frame() const;
    };
};
