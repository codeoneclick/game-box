//
//  animation_mixer.h
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef animation_mixer_h
#define animation_mixer_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class animation_mixer
    {
    private:
        
        f32 m_animation_time;
        f32 m_blending_animation_timeinterval;
        
        std::string m_current_animation_name;
        
        ui32 m_current_animation_frame;
        ui32 m_blending_animation_frame;
        
        sequence_shared_ptr m_previous_sequence;
        sequence_shared_ptr m_current_sequence;
        
        skeleton_shared_ptr m_skeleton;
        
        std::map<std::string, sequence_shared_ptr> m_sequences;
        bool m_is_binded;
        
        void bind_pose_transformation();
        bool bind_current_sequence();
        
    protected:
        
    public:
        
        animation_mixer(const skeleton_transfering_data_shared_ptr& skeleton_transfering_data,
                        const sequence_transfering_data_shared_ptr& bindpose_data);
        ~animation_mixer();
        
        glm::mat4* get_transformations() const;
        ui32 get_transformation_size() const;
        
        void add_sequence(const sequence_shared_ptr& sequence);
        void remove_sequence(const sequence_shared_ptr& sequence);
        
        void set_animation(const std::string& name);
        void update(f32 deltatime);
        
        bool is_animated() const;
    };
};

#endif
