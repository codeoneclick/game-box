//
//  sequence.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef sequence_h
#define sequence_h

#include "resource.h"

namespace gb
{
    extern const std::string k_bindpose_animation_name;
    
    class frame_data
    {
    private:
        
    protected:
        
        std::vector<glm::quat> m_rotations;
        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_scales;
        
    public:
        
        frame_data(const std::vector<glm::quat>& rotations,
                   const std::vector<glm::vec3>& positions,
                   const std::vector<glm::vec3>& scales);
        
        ~frame_data(void);
        
        const glm::quat& get_rotation(ui32 index) const;
        const glm::vec3& get_position(ui32 index) const;
        const glm::vec3& get_scale(ui32 index) const;
    };
    
    class sequence_transfering_data : public resource_transfering_data
    {
    private:
        
    protected:
        
        std::vector<std::shared_ptr<frame_data>> m_frames;
        ui32 m_animation_fps;
        std::string m_animation_name;
        
    public:
        
        sequence_transfering_data(const std::string& animation_name,
                                  ui32 animation_fps,
                                  const std::vector<std::shared_ptr<frame_data>>& frames);
        
        ~sequence_transfering_data(void);
        
        i32 get_num_frames(void) const;
        i32 get_animation_fps(void) const;
        
        const std::string get_animation_name(void) const;
        
        std::shared_ptr<frame_data> get_frame(ui32 index) const;
    };
    
    class sequence : public resource
    {
    private:
        
    protected:
        
        std::shared_ptr<sequence_transfering_data> m_data;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        sequence(const std::string& guid);
        
        static std::shared_ptr<sequence> construct(const std::string& guid,
                                                   const std::shared_ptr<sequence_transfering_data>& data);
        
        ~sequence(void);
        
        ui32 get_num_frames(void) const;
        ui32 get_animation_fps(void) const;
        const std::string get_animation_name(void) const;
        std::shared_ptr<frame_data> get_frame(ui32 index) const;
    };
}

#endif
