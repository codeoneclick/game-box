//
//  ces_animation_3d_system.h
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace gb
{
    class ces_animation_3d_system : public ces_base_system
    {
    private:
        
        void bind_pose_transformation(const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                                      const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component);
        
        bool try_bind_current_animation_sequence(const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                                                 const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component);
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        CTTI_CLASS_GUID(ces_animation_3d_system, ces_base_system::g_guids_container)
        ces_animation_3d_system();
        ~ces_animation_3d_system();
    };
};
