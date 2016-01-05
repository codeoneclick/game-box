//
//  animation_mixer.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "animation_mixer.h"
#include "mesh.h"
#include "bone.h"
#include "skeleton.h"
#include "sequence.h"
#include "glm_extensions.h"

#define k_blending_animation_timeinterval .25f
#define k_blending_animation_interpolation_multiplier 1.f / k_blending_animation_timeinterval

namespace gb
{
    animation_mixer::animation_mixer(const skeleton_transfering_data_shared_ptr& skeleton_transfering_data,
                                     const sequence_transfering_data_shared_ptr& bindpose_data) :
    m_animation_time(0.f),
    m_blending_animation_timeinterval(0.f),
    m_current_sequence(nullptr),
    m_previous_sequence(nullptr),
    m_current_animation_name(""),
    m_current_animation_frame(0),
    m_blending_animation_frame(0),
    m_is_binded(false)
    {
        assert(skeleton_transfering_data);
        assert(bindpose_data);
        m_skeleton = std::make_shared<skeleton>(skeleton_transfering_data);
        
        sequence_shared_ptr sequence = sequence::construct(k_bindpose_animation_name, bindpose_data);
        animation_mixer::add_sequence(sequence);
        animation_mixer::set_animation(k_bindpose_animation_name);
    }
    
    animation_mixer::~animation_mixer()
    {

    }
    
    void animation_mixer::bind_pose_transformation()
    {
        assert(m_skeleton);
        assert(m_current_sequence);
        
        std::shared_ptr<frame_data> frame = m_current_sequence->get_frame(0);
        bone_shared_ptr bone = nullptr;
        
        for (ui32 i = 0; i < m_skeleton->get_num_bones(); ++i)
        {
            glm::vec3 position = frame->get_position(i);
            glm::quat rotation = frame->get_rotation(i);
            glm::vec3 scale = frame->get_scale(i);
            
            glm::mat4 mat_translation = glm::translate(glm::mat4(1.f), position);
            glm::mat4 mat_rotation = glm::toMat4(rotation);
            glm::mat4 mat_scale = glm::scale(glm::mat4x4(1.f), scale);
            m_skeleton->get_bones_transformations()[i] = mat_translation * mat_rotation * mat_scale;
        }
        m_skeleton->update();
        m_skeleton->bind_pose_transformation();
        m_is_binded = true;
    }
    
    glm::mat4* animation_mixer::get_transformations() const
    {
        return m_skeleton->get_bones_transformations();
    }
    
    ui32 animation_mixer::get_transformation_size() const
    {
        return m_skeleton->get_num_bones();
    }
    
    bool animation_mixer::bind_current_sequence()
    {
        const auto& iterator = m_sequences.find(m_current_animation_name);
        if(iterator != m_sequences.end())
        {
            if(iterator->second->is_loaded())
            {
                m_current_sequence = iterator->second;
                if(!m_is_binded)
                {
                    animation_mixer::bind_pose_transformation();
                }
                return true;
            }
        }
        return false;
    }
    
    void animation_mixer::set_animation(const std::string& name)
    {
        if(m_current_animation_name != name)
        {
            m_current_animation_name = name;
            
            m_previous_sequence = m_current_sequence;
            m_blending_animation_frame = m_current_animation_frame;
            m_blending_animation_timeinterval = k_blending_animation_timeinterval;
            
            m_current_sequence = nullptr;
            animation_mixer::bind_current_sequence();
        }
    }
    
    void animation_mixer::update(f32 deltatime)
    {
        assert(m_skeleton != nullptr);
        
        if(m_current_animation_name.length() != 0)
        {
            bool is_current_sequence_binded = true;
            if(m_current_sequence == nullptr)
            {
                is_current_sequence_binded = animation_mixer::bind_current_sequence();
            }
            
            if(is_current_sequence_binded)
            {
                m_animation_time += deltatime;
                
                bool is_blending = false;
                if(m_blending_animation_timeinterval > 0.f && m_previous_sequence != nullptr)
                {
                    m_blending_animation_timeinterval -= deltatime;
                    is_blending = true;
                }
                else if(m_previous_sequence != nullptr)
                {
                    m_previous_sequence = nullptr;
                    m_animation_time = 0.f;
                }
                else
                {
                    m_previous_sequence = nullptr;
                }
                
                f32 animation_deltatime = m_animation_time * m_current_sequence->get_animation_fps();
                i32 floor_animation_deltatime = static_cast<i32>(floorf(animation_deltatime));
                
                i32 frame_index_01 = 0;
                i32 frame_index_02 = 0;
                f32 interpolation = 0.f;
                
                if(is_blending)
                {
                    frame_index_01 = m_blending_animation_frame;
                    frame_index_02 = 0;
                    interpolation = 1.f - m_blending_animation_timeinterval * k_blending_animation_interpolation_multiplier;
                    m_current_animation_frame = 0;
                }
                else
                {
                    frame_index_01 = floor_animation_deltatime % m_current_sequence->get_num_frames();
                    frame_index_02 = (frame_index_01 + 1) % m_current_sequence->get_num_frames();
                    m_current_animation_frame = frame_index_02;
                    interpolation = animation_deltatime - static_cast<f32>(floor_animation_deltatime);
                }
                
                i32 max_frame = is_blending ? m_previous_sequence->get_num_frames() - 1 : m_current_sequence->get_num_frames() - 1;
                frame_index_01 = std::min(frame_index_01, max_frame);
                max_frame = m_current_sequence->get_num_frames() - 1;
                frame_index_02 = std::min(frame_index_02, max_frame);
                
                std::shared_ptr<frame_data> frame_01 = is_blending ? m_previous_sequence->get_frame(frame_index_01) :
                m_current_sequence->get_frame(frame_index_01);
                std::shared_ptr<frame_data> frame_02 = m_current_sequence->get_frame(frame_index_02);
                
                for (ui32 i = 0; i < m_skeleton->get_num_bones(); ++i)
                {
                    glm::vec3 position = glm::mix(frame_01->get_position(i), frame_02->get_position(i), interpolation);
                    glm::quat rotation = glm::slerp(frame_01->get_rotation(i), frame_02->get_rotation(i), interpolation);
                    glm::vec3 scale = glm::mix(frame_01->get_scale(i), frame_02->get_scale(i), interpolation);
                    
                    glm::mat4x4 mat_translation = glm::translate(glm::mat4(1.f), position);
                    glm::mat4x4 mat_rotation = glm::toMat4(rotation);
                    glm::mat4x4 mat_scale = glm::scale(glm::mat4x4(1.f), scale);
                    m_skeleton->get_bones_transformations()[i] = mat_translation * mat_rotation * mat_scale;
                }
                m_skeleton->update();
            }
        }
    }
    
    bool animation_mixer::is_animated() const
    {
        return m_current_animation_name.length() != 0 && m_current_sequence != nullptr;
    }
    
    void animation_mixer::add_sequence(const sequence_shared_ptr &sequence)
    {
        std::string animation_name = sequence->get_animation_name();
        m_sequences.insert(std::make_pair(animation_name, sequence));
    }
    
    void animation_mixer::remove_sequence(const sequence_shared_ptr &sequence)
    {
        const auto& iterator = m_sequences.find(sequence->get_animation_name());
        if(iterator != m_sequences.end())
        {
            m_sequences.erase(iterator);
        }
    }
}