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

#define k_blending_animation_timeinterval .25f
#define k_blending_animation_interpolation_multiplier 1.f / k_blending_animation_timeinterval

namespace gb
{
    ces_animation_3d_mixer_component::ces_animation_3d_mixer_component() :
    m_animation_time(.0f),
    m_blending_animation_timeinterval(.0f),
    m_current_animation_sequence(nullptr),
    m_previous_animation_sequence(nullptr),
    m_bones_transformations(nullptr),
    m_current_animation_name(""),
    m_is_binded(false)
    {
        
    }
    
    ces_animation_3d_mixer_component::~ces_animation_3d_mixer_component()
    {
        
    }
    
    void ces_animation_3d_mixer_component::setup(const skeleton_3d_transfering_data_shared_ptr& skeleton_transfering_data,
                                                 const sequence_3d_transfering_data_shared_ptr& sequence_transfering_data)
    {
        m_bones_transformations = new glm::mat4[skeleton_transfering_data->get_num_bones()];
        animation_3d_sequence_shared_ptr sequence = animation_3d_sequence::construct(k_bindpose_animation_name,
                                                                                     sequence_transfering_data);
        ces_animation_3d_mixer_component::add_animation_sequence(sequence);
        ces_animation_3d_mixer_component::set_animation(k_bindpose_animation_name);
    }
    
    animation_3d_sequence_shared_ptr ces_animation_3d_mixer_component::get_current_animation_sequence() const
    {
        return m_current_animation_sequence;
    }
    
    void ces_animation_3d_mixer_component::bind_pose_transformation()
    {
        assert(m_current_animation_sequence != nullptr);
        
        frame_3d_data_shared_ptr frame = m_current_animation_sequence->get_frame(0);
        bone_3d_shared_ptr bone = nullptr;
        
        for (ui32 i = 0; i < m_skeleton->getNumBones(); ++i)
        {
            glm::vec3 position = frame->getPosition(i);
            glm::quat rotation = frame->getRotation(i);
            glm::vec3 scale = frame->getScale(i);
            
            glm::mat4x4 matrixTranslation = glm::translate(glm::mat4(1.0f), position);
            glm::mat4x4 matrixRotation = glm::toMat4(rotation);
            glm::mat4x4 matrixScale = glm::scale(glm::mat4x4(1.0f), scale);
            m_skeleton->getBonesTransformations()[i] = matrixTranslation * matrixRotation * matrixScale;
        }
        m_skeleton->update();
        m_skeleton->bindPoseTransformation();
        m_isBinded = true;
    }
    
    glm::mat4x4* CAnimationMixer::getTransformations(void) const
    {
        for(ui32 i = 0; i < m_skeleton->getNumBones(); ++i)
        {
            CSharedBone bone = m_skeleton->getBone(i);
            if(bone->getTransformation() != nullptr)
            {
                glm::mat4x4 boneTransformation = (*bone->getTransformation());
                m_bonesTransformations[i] = boneTransformation;
            }
            else
            {
                m_bonesTransformations[i] = glm::mat4x4(1.0f);
            }
        }
        return m_bonesTransformations;
    }
    
    ui32 CAnimationMixer::getTransformationSize(void) const
    {
        return m_skeleton->getNumBones();
    }
    
    bool CAnimationMixer::tryBindCurrentAnimationSequence(void)
    {
        const auto& iterator = m_animationsSequences.find(m_currentAnimationName);
        if(iterator != m_animationsSequences.end())
        {
            if(iterator->second->isLoaded())
            {
                m_currentAnimationSequence = iterator->second;
                if(!m_isBinded)
                {
                    CAnimationMixer::bindPoseTransformation();
                }
                return true;
            }
        }
        return false;
    }
    
    void CAnimationMixer::setAnimation(const std::string& name)
    {
        if(m_currentAnimationName != name)
        {
            m_currentAnimationName = name;
            
            m_previousAnimationSequence = m_currentAnimationSequence;
            m_blendingAnimationFrame = m_currentAnimationFrame;
            m_blendingAnimationTimeinterval = kBlendingAnimationTimeinterval;
            
            m_currentAnimationSequence = nullptr;
            CAnimationMixer::tryBindCurrentAnimationSequence();
        }
    }
    
    void CAnimationMixer::update(f32 deltatime)
    {
        assert(m_skeleton != nullptr);
        
        if(m_currentAnimationName.length() != 0)
        {
            bool isCurrentAnimationSequenceBinded = true;
            if(m_currentAnimationSequence == nullptr)
            {
                isCurrentAnimationSequenceBinded = CAnimationMixer::tryBindCurrentAnimationSequence();
            }
            
            if(isCurrentAnimationSequenceBinded)
            {
                m_animationTime += deltatime;
                
                bool isBlending = false;
                if(m_blendingAnimationTimeinterval > 0.0f && m_previousAnimationSequence != nullptr)
                {
                    m_blendingAnimationTimeinterval -= deltatime;
                    isBlending = true;
                }
                else if(m_previousAnimationSequence != nullptr)
                {
                    m_previousAnimationSequence = nullptr;
                    m_animationTime = 0.0f;
                }
                else
                {
                    m_previousAnimationSequence = nullptr;
                }
                
                f32 animationDeltaTime = m_animationTime * m_currentAnimationSequence->getAnimationFPS();
                i32 floorAnimationDeltaTime = static_cast<i32>(floorf(animationDeltaTime));
                
                i32 frameIndex_01 = 0;
                i32 frameIndex_02 = 0;
                f32 interpolation = 0.0f;
                
                if(isBlending)
                {
                    frameIndex_01 = m_blendingAnimationFrame;
                    frameIndex_02 = 0;
                    interpolation = 1.0f - m_blendingAnimationTimeinterval * kBlendingAnimationInterpolationMultiplier;
                    m_currentAnimationFrame = 0;
                }
                else
                {
                    frameIndex_01 = floorAnimationDeltaTime % m_currentAnimationSequence->getNumFrames();
                    frameIndex_02 = (frameIndex_01 + 1) % m_currentAnimationSequence->getNumFrames();
                    m_currentAnimationFrame = frameIndex_02;
                    interpolation = animationDeltaTime - static_cast<f32>(floorAnimationDeltaTime);
                }
                
                frameIndex_01 = MIN_VALUE(frameIndex_01, isBlending ? m_previousAnimationSequence->getNumFrames() - 1 :
                                          m_currentAnimationSequence->getNumFrames() - 1);
                frameIndex_02 = MIN_VALUE(frameIndex_02, m_currentAnimationSequence->getNumFrames() - 1);
                
                CSharedFrameData frame_01 = isBlending ? m_previousAnimationSequence->getFrame(frameIndex_01) :
                m_currentAnimationSequence->getFrame(frameIndex_01);
                CSharedFrameData frame_02 = m_currentAnimationSequence->getFrame(frameIndex_02);
                
                for (ui32 i = 0; i < m_skeleton->getNumBones(); ++i)
                {
                    glm::vec3 position = glm::mix(frame_01->getPosition(i), frame_02->getPosition(i), interpolation);
                    glm::quat rotation = glm::slerp(frame_01->getRotation(i), frame_02->getRotation(i), interpolation);
                    glm::vec3 scale = glm::mix(frame_01->getScale(i), frame_02->getScale(i), interpolation);
                    
                    glm::mat4x4 matrixTranslation = glm::translate(glm::mat4(1.0f), position);
                    glm::mat4x4 matrixRotation = glm::toMat4(rotation);
                    glm::mat4x4 matrixScale = glm::scale(glm::mat4x4(1.0f), scale);
                    m_skeleton->getBonesTransformations()[i] = matrixTranslation * matrixRotation * matrixScale;
                }
                m_skeleton->update();
            }
        }
    }
    
    bool CAnimationMixer::isAnimated(void)
    {
        return m_currentAnimationName.length() != 0 && m_currentAnimationSequence != nullptr;
    }
    
    void CAnimationMixer::addAnimationSequence(CSharedAnimationSequenceRef animationSequence)
    {
        std::string animationName = animationSequence->getAnimationName();
        m_animationsSequences.insert(std::make_pair(animationName, animationSequence));
    }
    
    void CAnimationMixer::removeAnimationSequence(CSharedAnimationSequenceRef animationSequence)
    {
        const auto& iterator = m_animationsSequences.find(animationSequence->getAnimationName());
        if(iterator != m_animationsSequences.end())
        {
            m_animationsSequences.erase(iterator);
        }
    }

}
