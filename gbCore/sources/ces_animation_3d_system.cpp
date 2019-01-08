//
//  ces_animation_3d_system.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_animation_3d_system.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_animation_3d_mask_component.h"
#include "ces_skeleton_3d_component.h"
#include "mesh_3d.h"
#include "bone_3d.h"
#include "animation_3d_sequence.h"

namespace gb
{
    ces_animation_3d_system::ces_animation_3d_system()
    {
        
    }
    
    ces_animation_3d_system::~ces_animation_3d_system()
    {
        
    }
    
    void ces_animation_3d_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_animation_3d_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        ces_animation_3d_system::update_recursively(root, dt);
    }
    
    void ces_animation_3d_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_animation_3d_system::update_recursively(const ces_entity_shared_ptr& entity, f32 dt)
    {
        auto animation_3d_mixer_component = entity->get_component<gb::ces_animation_3d_mixer_component>();
        auto animation_3d_mask_component = entity->get_component<gb::ces_animation_3d_mask_component>();
        auto skeleton_3d_component = entity->get_component<gb::ces_skeleton_3d_component>();
        if (animation_3d_mixer_component && skeleton_3d_component)
        {
            if (animation_3d_mixer_component->get_current_animation_name(0).length() != 0 && !animation_3d_mixer_component->get_is_animation_ended(0))
            {
                auto animation_sequence = animation_3d_mixer_component->get_current_animation_sequence(0);
                bool is_current_animation_sequence_binded = true;
                if (animation_sequence == nullptr)
                {
                    is_current_animation_sequence_binded = ces_animation_3d_system::validate_current_animation_sequence(animation_3d_mixer_component,
                                                                                                                        skeleton_3d_component,
                                                                                                                        0);
                    animation_sequence = animation_3d_mixer_component->get_current_animation_sequence(0);
                }
                
                if (is_current_animation_sequence_binded)
                {
                    i32 frame_index_01 = 0;
                    i32 frame_index_02 = 0;
                    
                    bool is_blending = false;
                    bool is_animation_ended = false;
                    f32 interpolation = .0f;
                   
                    update_animation_state(0, dt, animation_3d_mixer_component, skeleton_3d_component, &frame_index_01, &frame_index_02,
                                           &is_blending, &is_animation_ended, &interpolation);
                    
                    if(!is_animation_ended)
                    {
                        frame_index_01 = std::min(frame_index_01, is_blending ? animation_3d_mixer_component->get_previous_animation_sequence(0)->get_num_frames() - 1 :
                                                  animation_sequence->get_num_frames() - 1);
                        frame_index_02 = std::min(frame_index_02, animation_sequence->get_num_frames() - 1);
                        
                        frame_3d_data_shared_ptr frame_01 = is_blending ? animation_3d_mixer_component->get_previous_animation_sequence(0)->get_frame(frame_index_01) :
                        animation_sequence->get_frame(frame_index_01);
                        frame_3d_data_shared_ptr frame_02 = animation_sequence->get_frame(frame_index_02);
                        
                        i32 num_bones = skeleton_3d_component->get_num_bones();
                        bone_3d_shared_ptr bone = nullptr;
                        
                        for (i32 i = 0; i < num_bones; ++i)
                        {
                            glm::vec3 position = glm::mix(frame_01->get_position(i), frame_02->get_position(i), interpolation);
                            glm::quat rotation = glm::slerp(frame_01->get_rotation(i), frame_02->get_rotation(i), interpolation);
                            glm::vec3 scale = glm::mix(frame_01->get_scale(i), frame_02->get_scale(i), interpolation);
                            
                            glm::mat4x4 matrix_t = glm::translate(glm::mat4(1.f), position);
                            glm::mat4x4 matrix_r = glm::toMat4(rotation);
                            glm::mat4x4 matrix_s = glm::scale(glm::mat4x4(1.f), scale);
                            skeleton_3d_component->get_bones_transformations()[i] = matrix_t * matrix_r * matrix_s;
                            bone = skeleton_3d_component->get_bone(i);
                            bone->set_transformation(skeleton_3d_component->get_bones_transformations() + i);
                        }
                        
                        if (animation_3d_mixer_component->is_additional_animation_states_exist())
                        {
                            assert(animation_3d_mask_component != nullptr && "animation mask should be exist");
                            const auto additional_animation_states = animation_3d_mixer_component->get_additional_animation_states();
                            for (i32 i = 1; i <= additional_animation_states.size(); ++i)
                            {
                                auto additional_animation_state = additional_animation_states.at(i - 1);
                                animation_sequence = animation_3d_mixer_component->get_current_animation_sequence(i);
                                if (!animation_sequence)
                                {
                                    ces_animation_3d_system::validate_current_animation_sequence(animation_3d_mixer_component,
                                                                                                 skeleton_3d_component,
                                                                                                 i);
                                    animation_sequence = animation_3d_mixer_component->get_current_animation_sequence(i);
                                    assert(animation_sequence && "something went wrong with additional animation sequence");
                                }
                                
                                frame_index_01 = 0;
                                frame_index_02 = 0;
                                
                                is_blending = false;
                                is_animation_ended = false;
                                interpolation = .0f;
                                
                                update_animation_state(i, dt, animation_3d_mixer_component, skeleton_3d_component, &frame_index_01, &frame_index_02,
                                                       &is_blending, &is_animation_ended, &interpolation);
                                
                                frame_index_01 = std::min(frame_index_01, animation_sequence->get_num_frames() - 1);
                                frame_index_02 = std::min(frame_index_02, animation_sequence->get_num_frames() - 1);
                                
                                frame_01 = animation_sequence->get_frame(frame_index_01);
                                frame_02 = animation_sequence->get_frame(frame_index_02);
                                
                                for (i32 i = 0; i < num_bones; ++i)
                                {
                                    bone = skeleton_3d_component->get_bone(i);
                                    if (animation_3d_mask_component->is_bone_has_weight(additional_animation_state->get_animation_name(), bone->get_name()))
                                    {
                                        glm::vec3 position = glm::mix(frame_01->get_position(i), frame_02->get_position(i), interpolation);
                                        glm::quat rotation = glm::slerp(frame_01->get_rotation(i), frame_02->get_rotation(i), interpolation);
                                        glm::vec3 scale = glm::mix(frame_01->get_scale(i), frame_02->get_scale(i), interpolation);
                                        
                                        glm::mat4x4 matrix_t = glm::translate(glm::mat4(1.f), position);
                                        glm::mat4x4 matrix_r = glm::toMat4(rotation);
                                        glm::mat4x4 matrix_s = glm::scale(glm::mat4x4(1.f), scale);
                                        skeleton_3d_component->get_bones_transformations()[i] = matrix_t * matrix_r * matrix_s;
                                        
                                        bone->set_transformation(skeleton_3d_component->get_bones_transformations() + i);
                                    }
                                }
                            }
                        }
                        
                        for(const auto& root_bone : skeleton_3d_component->get_root_bones())
                        {
                            root_bone->update();
                        }
                        
                        auto bones_transformations = animation_3d_mixer_component->get_transformations();
                        for(i32 i = 0; i < num_bones; ++i)
                        {
                            bone = skeleton_3d_component->get_bone(i);
                            if(bone->get_transformation() != nullptr)
                            {
                                glm::mat4 bone_transformation = (*bone->get_transformation());
                                bones_transformations[i] = bone_transformation;
                            }
                            else
                            {
                                bones_transformations[i] = glm::mat4(1.f);
                            }
                        }
                    }
                }
            }
        }
        
        std::vector<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_animation_3d_system::update_recursively(child, dt);
        }
    }
    
    void ces_animation_3d_system::bind_pose(const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                                            const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component)
    {
        auto current_animation_sequence = animation_3d_mixer_component->get_current_animation_sequence(0);
        assert(current_animation_sequence != nullptr);
        
        frame_3d_data_shared_ptr frame = current_animation_sequence->get_frame(0);
        bone_3d_shared_ptr bone = nullptr;
        
        ui32 num_bones = skeleton_3d_component->get_num_bones();
        
        for (ui32 i = 0; i < num_bones; ++i)
        {
            glm::vec3 position = frame->get_position(i);
            glm::quat rotation = frame->get_rotation(i);
            glm::vec3 scale = frame->get_scale(i);
            
            glm::mat4x4 matrix_t = glm::translate(glm::mat4(1.0f), position);
            glm::mat4x4 matrix_r = glm::toMat4(rotation);
            glm::mat4x4 matrix_s = glm::scale(glm::mat4x4(1.0f), scale);
            skeleton_3d_component->get_bones_transformations()[i] = matrix_t * matrix_r * matrix_s;
            bone = skeleton_3d_component->get_bone(i);
            bone->set_transformation(skeleton_3d_component->get_bones_transformations() + i);
        }
        
        for(const auto& root_bone : skeleton_3d_component->get_root_bones())
        {
            root_bone->update();
        }
        
        for(const auto& root_bone : skeleton_3d_component->get_root_bones())
        {
            root_bone->bind_pose_transformation();
        }
        animation_3d_mixer_component->set_pose_binded(true);
    }
    
    bool ces_animation_3d_system::validate_current_animation_sequence(const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                                                                      const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component,
                                                                      i32 state)
    {
        return animation_3d_mixer_component->validate_current_animation_sequence(state);
    }
    
    void ces_animation_3d_system::update_animation_state(i32 state, f32 dt, const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                                                         const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component,
                                                         i32* frame_index_01, i32* frame_index_02,
                                                         bool* is_blending, bool* is_animation_ended, f32* interpolation)
    {
        const auto current_animation_sequence = animation_3d_mixer_component->get_current_animation_sequence(state);
        animation_3d_mixer_component->update_animation_time(state, dt);
        if (animation_3d_mixer_component->get_blending_animation_timeinterval(state) > .0f && animation_3d_mixer_component->get_previous_animation_sequence(state) != nullptr)
        {
            animation_3d_mixer_component->update_blending_animation_timeinterval(state, dt);
            *is_blending = true;
        }
        else if (animation_3d_mixer_component->get_previous_animation_sequence(state) != nullptr)
        {
            animation_3d_mixer_component->reset_previous_animation_sequence(state);
            animation_3d_mixer_component->reset_animation_time(state);
        }
        else
        {
            animation_3d_mixer_component->reset_previous_animation_sequence(state);
        }
        
        i32 animation_fps = animation_3d_mixer_component->get_cutom_animation_fps() != -1 ? animation_3d_mixer_component->get_cutom_animation_fps() : current_animation_sequence->get_animation_fps();
        f32 animation_dt = animation_3d_mixer_component->get_animation_time(state) * animation_fps;
        i32 floor_animation_dt = static_cast<i32>(floorf(animation_dt));
        
        if(*is_blending)
        {
            *frame_index_01 = animation_3d_mixer_component->get_blending_animation_frame(state);
            *frame_index_02 = 0;
            *interpolation = 1.f - animation_3d_mixer_component->get_blending_animation_timeinterval(state) * k_blending_animation_interpolation_multiplier;
            animation_3d_mixer_component->update_curret_animation_frame(state, 0);
        }
        else
        {
            *frame_index_01 = floor_animation_dt % current_animation_sequence->get_num_frames();
            *frame_index_02 = (*frame_index_01 + 1) % current_animation_sequence->get_num_frames();
            if(animation_3d_mixer_component->get_previous_played_frame(state) > *frame_index_02)
            {
                auto animation_ended_callbacks = animation_3d_mixer_component->get_animation_ended_callbacks();
                animation_ended_callbacks.erase(std::remove_if(animation_ended_callbacks.begin(), animation_ended_callbacks.end(), [state, animation_3d_mixer_component](const std::tuple<gb::ces_entity_weak_ptr,
                                                                                                                                                                  ces_animation_3d_mixer_component::on_animation_ended_callback_t>& it) {
                    if(!std::get<0>(it).expired())
                    {
                        std::get<1>(it)(std::get<0>(it).lock(),
                                        animation_3d_mixer_component->get_current_animation_name(state),
                                        animation_3d_mixer_component->get_is_looped(state));
                    }
                    return true;
                }), animation_ended_callbacks.end());
                
                if(!animation_3d_mixer_component->get_is_looped(state))
                {
                    animation_3d_mixer_component->interrupt_animation(state);
                    *is_animation_ended = true;
                }
            }
            if(!*is_animation_ended)
            {
                animation_3d_mixer_component->update_previous_played_frame(state, *frame_index_02);
                animation_3d_mixer_component->update_curret_animation_frame(state, *frame_index_02);
                *interpolation = animation_dt - static_cast<f32>(floor_animation_dt);
            }
        }
    }
}
