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
    class animation_3d_state
    {
    private:
        
    protected:
        
        std::string m_animation_name = "";
        
        f32 m_animation_time = 0.f;
        f32 m_blending_animation_timeinterval = 0.f;
       
        ui32 m_current_animation_frame = 0;
        ui32 m_previous_played_frame = 0;
        ui32 m_blending_animation_frame = 0;
        
        animation_3d_sequence_shared_ptr m_current_animation_sequence = nullptr;
        
        bool m_is_looped = false;
        bool m_is_animation_ended = false;
        
    public:
        
        animation_3d_sequence_shared_ptr get_current_animation_sequence() const;
        
        std::string get_animation_name() const;
        void set_animation_name(const std::string& animation_name);
        
        void reset();
        
        void set_current_animation_sequence(const animation_3d_sequence_shared_ptr& animation_sequence);
        
        bool get_is_looped() const;
        void set_is_looped(bool value);
        
        bool get_is_animation_ended() const;
        void set_is_animation_ended(bool value);
        
        f32 get_animation_time() const;
        void update_animation_time(f32 dt);
        void reset_animation_time();
        
        f32 get_blending_animation_timeinterval();
        void update_blending_animation_timeinterval(f32 dt);
        
        ui32 get_current_animation_frame() const;
        void update_current_animation_frame(ui32 frame);
        
        ui32 get_blending_animation_frame() const;
        
        ui32 get_previous_played_frame() const;
        void update_previous_player_frame(ui32 frame);
    };
    
    class ces_animation_3d_mixer_component : public ces_base_component
    {
    public:
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&, const std::string&, bool)> on_animation_ended_callback_t;
        
    private:
        
    protected:
        
        std::map<std::string, animation_3d_sequence_shared_ptr> m_animations_sequences;
        glm::mat4* m_bones_transformations;
        i32 m_num_bones_transformations;
        bool m_is_binded;
        
        i32 m_custom_animation_fps;
        
        frame_3d_data_shared_ptr m_blending_frame = nullptr;
        bool m_should_blend_with_previous_sequence = false;
        
        animation_3d_state_shared_ptr m_main_animation_state = nullptr;
        std::vector<animation_3d_state_shared_ptr> m_aditional_animation_states;
        
        std::vector<std::tuple<gb::ces_entity_weak_ptr, on_animation_ended_callback_t>> m_on_animation_ended_callbacks;

        void set_additional_animations(const std::vector<std::pair<std::string, bool>>& animations);
        animation_3d_state_shared_ptr get_additional_animation_state(i32 state) const;
        
        bool validate_animation_state(const animation_3d_state_shared_ptr& animation_state, bool include_bindpose = false);
        
    public:
        
        STTI_CLASS_GUID(ces_animation_3d_mixer_component, ces_base_component::g_guids_container)
        ces_animation_3d_mixer_component();
        ~ces_animation_3d_mixer_component();
        
        void setup(const skeleton_3d_transfering_data_shared_ptr& skeleton_transfering_data,
                   const sequence_3d_transfering_data_shared_ptr& sequence_transfering_data);
        
        void add_animation_sequence(const animation_3d_sequence_shared_ptr& sequence);
        void remove_animation_sequence(const animation_3d_sequence_shared_ptr& sequence);
        
        void set_animation(const std::string& main_animation_name, bool is_looped = false, const std::vector<std::pair<std::string, bool>>& additional_animations = std::vector<std::pair<std::string, bool>>());
        bool is_animated();
        
        void set_pose_binded(bool value);
        bool get_pose_binded() const;
        
        bool validate_current_animation_sequence(i32 state);
        
        std::string get_current_animation_name(i32 state) const;
        
        bool get_is_looped(i32 state) const;
        
        void update_animation_time(i32 state, f32 dt);
        f32 get_animation_time(i32 state) const;
        
        void update_blending_animation_timeinterval(i32 state, f32 dt);
        f32 get_blending_animation_timeinterval(i32 state) const;
        
        void reset_animation_time(i32 state);
        
        ui32 get_current_animation_frame(i32 state) const;
        ui32 get_blending_animation_frame(i32 state) const;
        
        void update_curret_animation_frame(i32 state, ui32 frame);
        
        animation_3d_sequence_shared_ptr get_current_animation_sequence(i32 state) const;
        
        glm::mat4* get_transformations() const;
        i32 get_transformation_size() const;
        
        bool is_animation_ended_callback_exist(const gb::ces_entity_shared_ptr& owner);
        void add_animation_ended_callback(const ces_entity_shared_ptr& owner, const on_animation_ended_callback_t& callback);
        void remove_animation_ended_callback(const ces_entity_shared_ptr& owner);
        const std::vector<std::tuple<gb::ces_entity_weak_ptr, on_animation_ended_callback_t>>& get_animation_ended_callbacks() const;
        
        void interrupt_animation(i32 state);
        bool get_is_animation_ended(i32 state) const;
        
        void set_custom_animation_fps(i32 fps);
        i32 get_cutom_animation_fps() const;
        
        void update_previous_played_frame(i32 state, ui32 frame);
        ui32 get_previous_played_frame(i32 state) const;
        
        bool is_additional_animation_states_exist() const;
        const std::vector<animation_3d_state_shared_ptr>& get_additional_animation_states() const;
        
        void update_blending_frame_position(ui32 index, const glm::vec3& position);
        void update_blending_frame_rotation(ui32 index, const glm::quat& rotation);
        void update_blending_frame_scale(ui32 index, const glm::vec3& scale);
        
        frame_3d_data_shared_ptr get_blending_frame() const;
        
        bool get_should_blend_with_previous_sequence() const;
        void set_should_blend_with_previous_sequence(bool value);
    };
};
