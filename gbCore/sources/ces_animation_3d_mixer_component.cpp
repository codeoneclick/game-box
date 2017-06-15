//
//  ces_animation_3d_mixer_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_animation_3d_mixer_component.h"
#include "mesh_3d.h"
#include "animation_sequence_3d.h"

namespace gb
{
    i32 ces_animation_3d_mixer_component::g_animation_end_callback_id = 0;
    
    ces_animation_3d_mixer_component::ces_animation_3d_mixer_component() :
    m_animation_time(.0f),
    m_blending_animation_timeinterval(.0f),
    m_current_animation_sequence(nullptr),
    m_previous_animation_sequence(nullptr),
    m_bones_transformations(nullptr),
    m_num_bones_transformations(0),
    m_current_animation_name(""),
	m_current_animation_frame(0),
	m_blending_animation_frame(0),
    m_is_binded(false),
    m_is_looped(false),
    m_is_animation_ended(false),
    m_custom_animation_fps(-1),
    m_previous_played_frame(0)
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
        ces_animation_3d_mixer_component::validate_current_animation_sequence();
    }
    
    animation_3d_sequence_shared_ptr ces_animation_3d_mixer_component::get_current_animation_sequence() const
    {
        return m_current_animation_sequence;
    }
    
    animation_3d_sequence_shared_ptr ces_animation_3d_mixer_component::get_previous_animation_sequence() const
    {
        return m_previous_animation_sequence;
    }
    
    void ces_animation_3d_mixer_component::set_pose_binded(bool value)
    {
        m_is_binded = value;
    }
    
    bool ces_animation_3d_mixer_component::get_pose_binded() const
    {
        return m_is_binded;
    }
    
    bool ces_animation_3d_mixer_component::validate_current_animation_sequence()
    {
        auto animation_name_linkage = m_animation_names_linkage.find(m_current_animation_name);
        if(animation_name_linkage != m_animation_names_linkage.end())
        {
            const auto& iterator = m_animations_sequences.find(animation_name_linkage->second);
            if(iterator != m_animations_sequences.end())
            {
                if(iterator->second->is_loaded())
                {
                    m_current_animation_sequence = iterator->second;
                    return true;
                }
            }
        }
        else if(m_current_animation_name == k_bindpose_animation_name)
        {
            const auto& iterator = m_animations_sequences.find(k_bindpose_animation_name);
            if(iterator != m_animations_sequences.end())
            {
                if(iterator->second->is_loaded())
                {
                    m_current_animation_sequence = iterator->second;
                    return true;
                }
            }
        }
        return false;
    }
    
    std::string ces_animation_3d_mixer_component::get_current_animation_name() const
    {
        return m_current_animation_name;
    }
    
    bool ces_animation_3d_mixer_component::get_is_looped() const
    {
        return m_is_looped;
    }
    
    void ces_animation_3d_mixer_component::set_animation(const std::string& animation_name, bool is_looped)
    {
        if(m_current_animation_name != animation_name || m_is_animation_ended)
        {
            auto animation_name_linkage = m_animation_names_linkage.find(animation_name);
            if(animation_name_linkage != m_animation_names_linkage.end())
            {
                m_current_animation_name = animation_name_linkage->first;
            }
            else if(animation_name == k_bindpose_animation_name)
            {
                m_current_animation_name = animation_name;
            }
            else
            {
                assert(false);
            }
            
            m_previous_animation_sequence = m_current_animation_sequence;
            m_blending_animation_frame = m_current_animation_frame;
            m_blending_animation_timeinterval = k_blending_animation_timeinterval;
            m_current_animation_sequence = nullptr;
            m_is_looped = is_looped;
            m_is_animation_ended = false;
            m_previous_played_frame = 0;
        }
    }
    
    void ces_animation_3d_mixer_component::update_animation_time(f32 dt)
    {
        m_animation_time += dt;
    }
    
    f32 ces_animation_3d_mixer_component::get_animation_time() const
    {
        return m_animation_time;
    }
    
    void ces_animation_3d_mixer_component::update_blending_animation_timeinterval(f32 dt)
    {
        m_blending_animation_timeinterval -= dt;
    }
    
    f32 ces_animation_3d_mixer_component::get_blending_animation_timeinterval() const
    {
        return m_blending_animation_timeinterval;
    }
    
    void ces_animation_3d_mixer_component::reset_animation_time()
    {
        m_animation_time = 0.f;
    }
    
    void ces_animation_3d_mixer_component::reset_previous_animation_sequence()
    {
        m_previous_animation_sequence = nullptr;
    }
    
    ui32 ces_animation_3d_mixer_component::get_current_animation_frame() const
    {
        return m_current_animation_frame;
    }
    
    ui32 ces_animation_3d_mixer_component::get_blending_animation_frame() const
    {
        return m_blending_animation_frame;
    }
    
    void ces_animation_3d_mixer_component::update_curret_animation_frame(ui32 current_frame)
    {
        m_current_animation_frame = current_frame;
    }
    
    bool ces_animation_3d_mixer_component::is_animated()
    {
        return m_current_animation_name.length() != 0 && m_current_animation_sequence != nullptr;
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
    
    void ces_animation_3d_mixer_component::add_animation_name_linkage(const std::string& animation_name, const std::string& filename)
    {
        m_animation_names_linkage[animation_name] = filename;
    }
    
    i32 ces_animation_3d_mixer_component::add_animation_end_callback(const animation_end_callback_t& callback)
    {
        m_animation_end_callbacks.insert(std::make_pair(g_animation_end_callback_id, callback));
        g_animation_end_callback_id++;
        return g_animation_end_callback_id - 1;
    }
    
    void ces_animation_3d_mixer_component::remove_animation_end_callback(i32 id)
    {
        auto it = m_animation_end_callbacks.find(id);
        if(it != m_animation_end_callbacks.end())
        {
            m_animation_end_callbacks.erase(it);
        }
    }
    
    const std::map<i32, ces_animation_3d_mixer_component::animation_end_callback_t>& ces_animation_3d_mixer_component::get_animation_end_callbacks() const
    {
        return m_animation_end_callbacks;
    }
    
    void ces_animation_3d_mixer_component::interrupt_animation()
    {
        m_is_animation_ended = true;
    }
    
    bool ces_animation_3d_mixer_component::get_is_animation_ended() const
    {
        return m_is_animation_ended;
    }
    
    void ces_animation_3d_mixer_component::set_custom_animation_fps(i32 fps)
    {
        m_custom_animation_fps = fps;
    }
    
    i32 ces_animation_3d_mixer_component::get_cutom_animation_fps() const
    {
        return m_custom_animation_fps;
    }
    
    void ces_animation_3d_mixer_component::set_previous_played_frame(i32 frame)
    {
        m_previous_played_frame = frame;
    }
    
    i32 ces_animation_3d_mixer_component::get_previous_played_frame() const
    {
        return m_previous_played_frame;
    }
}
