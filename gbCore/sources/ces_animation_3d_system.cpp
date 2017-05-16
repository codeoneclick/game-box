//
//  ces_animation_3d_system.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_animation_3d_system.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_skeleton_3d_component.h"
#include "mesh_3d.h"
#include "animation_sequence_3d.h"

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
        auto skeleton_3d_component = entity->get_component<gb::ces_skeleton_3d_component>();
        if (animation_3d_mixer_component && skeleton_3d_component)
        {

        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_animation_3d_system::update_recursively(child, dt);
        }
    }
    
    void ces_animation_3d_system::bind_pose_transformation(const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                                                           const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component)
    {
        auto current_animation_sequence = animation_3d_mixer_component->get_current_animation_sequence();
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
        }
        m_skeleton->update();
        m_skeleton->bindPoseTransformation();
        m_isBinded = true;
    }

}
