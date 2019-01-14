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
        
        bool validate_current_animation_sequence(const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                                                 const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component,
                                                 i32 state);
        
        void update_animation_state(i32 state, f32 dt, const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                                    const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component,
                                    i32* frame_index_01, i32* frame_index_02,
                                    bool* is_blending, bool* is_ended, f32* interpolation);
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 dt);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const ces_entity_shared_ptr& entity, f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_animation_3d_system, ces_base_system::g_guids_container)
        ces_animation_3d_system();
        ~ces_animation_3d_system();
        
        static void bind_pose(const ces_animation_3d_mixer_component_shared_ptr& animation_3d_mixer_component,
                              const ces_skeleton_3d_component_shared_ptr& skeleton_3d_component);
    };
};
