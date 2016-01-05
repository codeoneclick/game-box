//
//  sequence.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "sequence.h"
#include "resource_status.h"

namespace gb
{
    const std::string k_bindpose_animation_name = "bindpose";
    
    frame_data::frame_data(const std::vector<glm::quat>& rotations,
                           const std::vector<glm::vec3>& positions,
                           const std::vector<glm::vec3>& scales) :
    m_rotations(rotations),
    m_positions(positions),
    m_scales(scales)
    {
        
    }
    
    frame_data::~frame_data(void)
    {
        m_rotations.clear();
        m_positions.clear();
        m_scales.clear();
    }
    
    const glm::quat& frame_data::get_rotation(ui32 index) const
    {
        assert(m_rotations.size() > index);
        return m_rotations.at(index);
    }
    
    const glm::vec3& frame_data::get_position(ui32 index) const
    {
        assert(m_positions.size() > index);
        return m_positions.at(index);
    }
    
    const glm::vec3& frame_data::get_scale(ui32 index) const
    {
        assert(m_scales.size() > index);
        return m_scales.at(index);
    }
    
    sequence_transfering_data::sequence_transfering_data(const std::string& animation_name,
                                                         ui32 animation_fps,
                                                         const std::vector<std::shared_ptr<frame_data>>& frames) :
    m_animation_name(animation_name),
    m_animation_fps(animation_fps),
    m_frames(frames)
    {
        m_type = e_resource_transfering_data_type_sequence;
    }
    
    sequence_transfering_data::~sequence_transfering_data(void)
    {
        m_frames.clear();
    }
    
    i32 sequence_transfering_data::get_num_frames(void) const
    {
        return static_cast<ui32>(m_frames.size());
    }
    
    i32 sequence_transfering_data::get_animation_fps(void) const
    {
        return m_animation_fps;
    }
    
    const std::string sequence_transfering_data::get_animation_name(void) const
    {
        return m_animation_name;
    }
    
    std::shared_ptr<frame_data> sequence_transfering_data::get_frame(ui32 index) const
    {
        assert(m_frames.size() > index);
        if(index >= m_frames.size())
        {
            index = static_cast<ui32>(m_frames.size()) - 1;
        }
        return m_frames.at(index);
    }
    
    sequence::sequence(const std::string& guid) :
    gb::resource(e_resource_type_animation, guid),
    m_data(nullptr)
    {
        
    }
    
    std::shared_ptr<sequence> sequence::construct(const std::string& guid,
                                                  const std::shared_ptr<sequence_transfering_data>& data)
    {
        std::shared_ptr<sequence> sequence = std::make_shared<gb::sequence>(guid);
        sequence->on_transfering_data_serialized(data);
        sequence->on_transfering_data_commited(data);
        return sequence;
    }
    
    sequence::~sequence(void)
    {
        
    }
    
    void sequence::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_sequence:
            {
                m_data = std::static_pointer_cast<sequence_transfering_data>(data);
                m_status |= e_resource_status_loaded;
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    void sequence::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_sequence:
            {
                m_status |= e_resource_status_commited;
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    ui32 sequence::get_num_frames(void) const
    {
        return resource::is_loaded() ? m_data->get_num_frames() : 0;
    }
    
    ui32 sequence::get_animation_fps(void) const
    {
        return resource::is_loaded() ? m_data->get_animation_fps() : 0;
    }
    
    const std::string sequence::get_animation_name(void) const
    {
        return resource::is_loaded() ? m_data->get_animation_name() : "";
    }
    
    std::shared_ptr<frame_data> sequence::get_frame(ui32 index) const
    {
        return resource::is_loaded() ? m_data->get_frame(index) : nullptr;
    }
}