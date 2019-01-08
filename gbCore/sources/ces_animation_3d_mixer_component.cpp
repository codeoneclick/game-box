//
//  ces_animation_3d_mixer_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_animation_3d_mixer_component.h"
#include "mesh_3d.h"
#include "animation_3d_sequence.h"

namespace gb
{
    
    animation_3d_sequence_shared_ptr animation_3d_state::get_current_animation_sequence() const
    {
        return m_current_animation_sequence;
    }
    
    animation_3d_sequence_shared_ptr animation_3d_state::get_previous_animation_sequence() const
    {
        return m_previous_animation_sequence;
    }
    
    void animation_3d_state::reset()
    {
        m_previous_animation_sequence = m_current_animation_sequence;
        m_blending_animation_frame = m_current_animation_frame;
        m_blending_animation_timeinterval = k_blending_animation_timeinterval;
        m_current_animation_sequence = nullptr;
        m_is_animation_ended = false;
        m_previous_played_frame = 0;
    }
    
    std::string animation_3d_state::get_animation_name() const
    {
        return m_animation_name;
    }
    
    void animation_3d_state::set_animation_name(const std::string& animation_name)
    {
        m_animation_name = animation_name;
    }
    
    void animation_3d_state::set_current_animation_sequence(const animation_3d_sequence_shared_ptr& sequence)
    {
        m_current_animation_sequence = sequence;
    }
    
    void animation_3d_state::set_previous_animation_sequence(const animation_3d_sequence_shared_ptr& sequence)
    {
        m_previous_animation_sequence = sequence;
    }
    
    bool animation_3d_state::get_is_looped() const
    {
        return m_is_looped;
    }
    
    void animation_3d_state::set_is_looped(bool value)
    {
        m_is_looped = value;
    }
    
    bool animation_3d_state::get_is_animation_ended() const
    {
        return m_is_animation_ended;
    }
    
    void animation_3d_state::set_is_animation_ended(bool value)
    {
        m_is_animation_ended = value;
    }
    
    f32 animation_3d_state::get_animation_time() const
    {
        return m_animation_time;
    }
    
    void animation_3d_state::update_animation_time(f32 dt)
    {
        m_animation_time += dt;
    }
    
    void animation_3d_state::reset_animation_time()
    {
        m_animation_time = 0.f;
    }
    
    f32 animation_3d_state::get_blending_animation_timeinterval()
    {
        return m_blending_animation_timeinterval;
    }
    
    void animation_3d_state::update_blending_animation_timeinterval(f32 dt)
    {
        m_blending_animation_timeinterval -= dt;
    }
    
    ui32 animation_3d_state::get_current_animation_frame() const
    {
        return m_current_animation_frame;
    }
    
    void animation_3d_state::update_current_animation_frame(ui32 frame)
    {
        m_current_animation_frame = frame;
    }
    
    ui32 animation_3d_state::get_blending_animation_frame() const
    {
        return m_blending_animation_frame;
    }
    
    ui32 animation_3d_state::get_previous_played_frame() const
    {
        return m_previous_played_frame;
    }
    
    void animation_3d_state::update_previous_player_frame(ui32 frame)
    {
        m_previous_played_frame = frame;
    }
    
    ces_animation_3d_mixer_component::ces_animation_3d_mixer_component() :
    m_bones_transformations(nullptr),
    m_num_bones_transformations(0),
    m_is_binded(false),
    m_custom_animation_fps(-1),
    m_main_animation_state(std::make_shared<animation_3d_state>())
    {
        
    }
    
    ces_animation_3d_mixer_component::~ces_animation_3d_mixer_component()
    {
        
    }
    
    void ces_animation_3d_mixer_component::setup(const skeleton_3d_transfering_data_shared_ptr& skeleton_transfering_data,
                                                 const sequence_3d_transfering_data_shared_ptr& sequence_transfering_data)
    {
        m_num_bones_transformations = skeleton_transfering_data->get_num_bones();
        m_bones_transformations = new glm::mat4[m_num_bones_transformations];
        animation_3d_sequence_shared_ptr sequence = animation_3d_sequence::construct(k_bindpose_animation_name,
                                                                                     sequence_transfering_data);
        ces_animation_3d_mixer_component::add_animation_sequence(sequence);
        ces_animation_3d_mixer_component::set_animation(k_bindpose_animation_name);
        ces_animation_3d_mixer_component::validate_current_animation_sequence(0);
    }
    
    animation_3d_sequence_shared_ptr ces_animation_3d_mixer_component::get_current_animation_sequence(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_current_animation_sequence();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_current_animation_sequence();
            }
            else
            {
                assert(false);
            }
        }
        return nullptr;
    }
    
    animation_3d_sequence_shared_ptr ces_animation_3d_mixer_component::get_previous_animation_sequence(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_previous_animation_sequence();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_previous_animation_sequence();
            }
            else
            {
                assert(false);
            }
        }
        return nullptr;
    }
    
    void ces_animation_3d_mixer_component::set_pose_binded(bool value)
    {
        m_is_binded = value;
    }
    
    bool ces_animation_3d_mixer_component::get_pose_binded() const
    {
        return m_is_binded;
    }
    
    bool ces_animation_3d_mixer_component::validate_animation_state(const animation_3d_state_shared_ptr& animation_state, bool include_bindpose)
    {
        if(animation_state->get_animation_name() == k_bindpose_animation_name && include_bindpose)
        {
            const auto& iterator = m_animations_sequences.find(k_bindpose_animation_name);
            if(iterator != m_animations_sequences.end())
            {
                if(iterator->second->is_loaded())
                {
                    animation_state->set_current_animation_sequence(iterator->second);
                    return true;
                }
            }
        }
        
        const auto& iterator = m_animations_sequences.find(animation_state->get_animation_name());
        if(iterator != m_animations_sequences.end())
        {
            if(iterator->second->is_loaded())
            {
                animation_state->set_current_animation_sequence(iterator->second);
                return true;
            }
        }
        return false;
    }
    
    bool ces_animation_3d_mixer_component::validate_current_animation_sequence(i32 state)
    {
        if (state == 0)
        {
            validate_animation_state(m_main_animation_state, true);
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                validate_animation_state(animation_state);
            }
            else
            {
                assert(false);
            }
        }
        return false;
    }
    
    std::string ces_animation_3d_mixer_component::get_current_animation_name(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_animation_name();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_animation_name();
            }
            else
            {
                assert(false);
            }
        }
        return "";
    }
    
    bool ces_animation_3d_mixer_component::get_is_looped(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_is_looped();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_is_looped();
            }
            else
            {
                assert(false);
            }
        }
        return false;
    }
    
    void ces_animation_3d_mixer_component::set_animation(const std::string& main_animation_name, bool is_looped, const std::vector<std::pair<std::string, bool>>& additional_animations)
    {
        if(m_main_animation_state->get_animation_name() != main_animation_name ||
           m_main_animation_state->get_is_animation_ended())
        {
            const auto& iterator = m_animations_sequences.find(main_animation_name);
            if (iterator != m_animations_sequences.end() || main_animation_name == k_bindpose_animation_name)
            {
                 m_main_animation_state->set_animation_name(main_animation_name);
            }
            else
            {
                assert(false);
            }
            m_main_animation_state->reset();
            m_main_animation_state->set_is_looped(is_looped);
        }
        
        if (additional_animations.size() != 0)
        {
            if (m_aditional_animation_states.size() == 0)
            {
                set_additional_animations(additional_animations);
            }
            else if (m_aditional_animation_states.size() == additional_animations.size())
            {
                for (i32 i = 0; i < additional_animations.size(); ++i)
                {
                    if (additional_animations[i].first != m_aditional_animation_states[i]->get_animation_name() ||
                        m_aditional_animation_states[i]->get_is_animation_ended())
                    {
                        m_aditional_animation_states.clear();
                        set_additional_animations(additional_animations);
                        break;
                    }
                }
            }
        }
    }
    
    void ces_animation_3d_mixer_component::set_additional_animations(const std::vector<std::pair<std::string, bool>>& animations)
    {
        for (auto additional_animation_it : animations)
        {
            const auto& iterator = m_animations_sequences.find(additional_animation_it.first);
            if (iterator != m_animations_sequences.end())
            {
                auto aditional_animation_state = std::make_shared<animation_3d_state>();
                aditional_animation_state->set_animation_name(additional_animation_it.first);
                aditional_animation_state->reset();
                aditional_animation_state->set_is_looped(additional_animation_it.second);
                m_aditional_animation_states.push_back(aditional_animation_state);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    animation_3d_state_shared_ptr ces_animation_3d_mixer_component::get_additional_animation_state(i32 state) const
    {
        animation_3d_state_shared_ptr animation_state = nullptr;
        if (state - 1 >= 0 && state - 1 < m_aditional_animation_states.size())
        {
            animation_state = m_aditional_animation_states.at(state - 1);
        }
        else
        {
            assert(false);
        }
        return animation_state;
    }
    
    void ces_animation_3d_mixer_component::update_animation_time(i32 state, f32 dt)
    {
        if (state == 0)
        {
            m_main_animation_state->update_animation_time(dt);
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                animation_state->update_animation_time(dt);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    f32 ces_animation_3d_mixer_component::get_animation_time(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_animation_time();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_animation_time();
            }
            else
            {
                assert(false);
            }
        }
        return 0;
    }
    
    void ces_animation_3d_mixer_component::update_blending_animation_timeinterval(i32 state, f32 dt)
    {
        if (state == 0)
        {
            m_main_animation_state->update_blending_animation_timeinterval(dt);
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                animation_state->update_blending_animation_timeinterval(dt);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    f32 ces_animation_3d_mixer_component::get_blending_animation_timeinterval(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_blending_animation_timeinterval();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_blending_animation_timeinterval();
            }
            else
            {
                assert(false);
            }
        }
        return 0;
    }
    
    void ces_animation_3d_mixer_component::reset_animation_time(i32 state)
    {
        if (state == 0)
        {
            m_main_animation_state->reset_animation_time();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                animation_state->reset_animation_time();
            }
            else
            {
                assert(false);
            }
        }
    }
    
    void ces_animation_3d_mixer_component::reset_previous_animation_sequence(i32 state)
    {
        if (state == 0)
        {
            m_main_animation_state->set_previous_animation_sequence(nullptr);
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                animation_state->set_previous_animation_sequence(nullptr);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    ui32 ces_animation_3d_mixer_component::get_current_animation_frame(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_current_animation_frame();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_current_animation_frame();
            }
            else
            {
                assert(false);
            }
        }
        return 0;
    }
    
    ui32 ces_animation_3d_mixer_component::get_blending_animation_frame(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_blending_animation_frame();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_blending_animation_frame();
            }
            else
            {
                assert(false);
            }
        }
        return 0;
    }
    
    void ces_animation_3d_mixer_component::update_curret_animation_frame(i32 state, ui32 frame)
    {
        if (state == 0)
        {
            m_main_animation_state->update_current_animation_frame(frame);
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->update_current_animation_frame(frame);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    bool ces_animation_3d_mixer_component::is_animated()
    {
        return m_main_animation_state->get_animation_name().length() != 0 && m_main_animation_state->get_current_animation_sequence() != nullptr;
    }
    
    void ces_animation_3d_mixer_component::add_animation_sequence(const animation_3d_sequence_shared_ptr& sequence)
    {
        std::string animation_name = sequence->get_animation_name();
        m_animations_sequences.insert(std::make_pair(animation_name, sequence));
    }
    
    void ces_animation_3d_mixer_component::remove_animation_sequence(const animation_3d_sequence_shared_ptr& sequence)
    {
        const auto& iterator = m_animations_sequences.find(sequence->get_animation_name());
        if(iterator != m_animations_sequences.end())
        {
            m_animations_sequences.erase(iterator);
        }
    }
    
    glm::mat4* ces_animation_3d_mixer_component::get_transformations() const
    {
        return m_bones_transformations;
    }
    
    i32 ces_animation_3d_mixer_component::get_transformation_size() const
    {
        return m_num_bones_transformations;
    }
    
    bool ces_animation_3d_mixer_component::is_animation_ended_callback_exist(const gb::ces_entity_shared_ptr& owner)
    {
        bool is_exist = false;
        for(const auto& it : m_on_animation_ended_callbacks)
        {
            if(!std::get<0>(it).expired())
            {
                if(owner == std::get<0>(it).lock())
                {
                    is_exist = true;
                    break;
                }
            }
        }
        return is_exist;
    }
    
    void ces_animation_3d_mixer_component::add_animation_ended_callback(const ces_entity_shared_ptr& owner, const on_animation_ended_callback_t& callback)
    {
        m_on_animation_ended_callbacks.push_back(std::make_tuple(owner, callback));
    }
    
    void ces_animation_3d_mixer_component::remove_animation_ended_callback(const ces_entity_shared_ptr& owner)
    {
        m_on_animation_ended_callbacks.erase(std::remove_if(m_on_animation_ended_callbacks.begin(), m_on_animation_ended_callbacks.end(), [owner](const std::tuple<gb::ces_entity_weak_ptr, on_animation_ended_callback_t>& it) {
            if(!std::get<0>(it).expired())
            {
                return owner == std::get<0>(it).lock();
            }
            return true;
        }), m_on_animation_ended_callbacks.end());
    }
    
    const std::vector<std::tuple<gb::ces_entity_weak_ptr, ces_animation_3d_mixer_component::on_animation_ended_callback_t>>& ces_animation_3d_mixer_component::get_animation_ended_callbacks() const
    {
        return m_on_animation_ended_callbacks;
    }
    
    void ces_animation_3d_mixer_component::interrupt_animation(i32 state)
    {
        if (state == 0)
        {
            m_main_animation_state->set_is_animation_ended(true);
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                animation_state->set_is_animation_ended(true);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    bool ces_animation_3d_mixer_component::get_is_animation_ended(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_is_animation_ended();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_is_animation_ended();
            }
            else
            {
                assert(false);
            }
        }
        return false;
    }
    
    void ces_animation_3d_mixer_component::set_custom_animation_fps(i32 fps)
    {
        m_custom_animation_fps = fps;
    }
    
    i32 ces_animation_3d_mixer_component::get_cutom_animation_fps() const
    {
        return m_custom_animation_fps;
    }
    
    void ces_animation_3d_mixer_component::update_previous_played_frame(i32 state, ui32 frame)
    {
        if (state == 0)
        {
            m_main_animation_state->update_previous_player_frame(frame);
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                animation_state->update_previous_player_frame(frame);
            }
            else
            {
                assert(false);
            }
        }
    }
    
    ui32 ces_animation_3d_mixer_component::get_previous_played_frame(i32 state) const
    {
        if (state == 0)
        {
            return m_main_animation_state->get_previous_played_frame();
        }
        else
        {
            animation_3d_state_shared_ptr animation_state = get_additional_animation_state(state);
            if (animation_state)
            {
                return animation_state->get_previous_played_frame();
            }
            else
            {
                assert(false);
            }
        }
        return 0;
    }
}
