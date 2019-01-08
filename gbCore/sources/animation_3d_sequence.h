//
//  animation_3d_sequence.h
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"

namespace gb
{
    extern const std::string k_bindpose_animation_name;
    
    class frame_3d_data final
    {
    private:
        
    protected:
        
        std::vector<glm::quat> m_rotations;
        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_scales;
        
    public:
        
        frame_3d_data(const std::vector<glm::quat>& rotations,
                      const std::vector<glm::vec3>& positions,
                      const std::vector<glm::vec3>& scales);
        
        ~frame_3d_data();
        
        const glm::quat& get_rotation(ui32 index) const;
        const glm::vec3& get_position(ui32 index) const;
        const glm::vec3& get_scale(ui32 index) const;
    };
    
    class sequence_3d_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        std::vector<frame_3d_data_shared_ptr> m_frames;
        i32 m_animation_fps;
        std::string m_animation_name;
        
    public:
        
        sequence_3d_transfering_data(const std::string& animation_name,
                                     i32 animation_fps,
                                     const std::vector<frame_3d_data_shared_ptr>& frames);
        
        ~sequence_3d_transfering_data();
        
        i32 get_num_frames() const;
        i32 get_animation_fps() const;
        const std::string get_animation_name() const;
        frame_3d_data_shared_ptr get_frame(i32 index) const;
    };
    
    class animation_3d_sequence : public resource
    {
    private:
        
    protected:
        
        sequence_3d_transfering_data_shared_ptr m_sequence_data;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        animation_3d_sequence(const std::string& guid);
        
        static animation_3d_sequence_shared_ptr construct(const std::string& guid,
                                                          const sequence_3d_transfering_data_shared_ptr& data);
        
        ~animation_3d_sequence();
        
        i32 get_num_frames() const;
        i32 get_animation_fps() const;
        const std::string get_animation_name() const;
        frame_3d_data_shared_ptr get_frame(i32 index) const;
    };
};
