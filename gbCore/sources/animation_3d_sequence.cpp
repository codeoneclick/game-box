//
//  animation_sequence_3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "animation_3d_sequence.h"
#include "resource_status.h"

namespace gb
{
    const std::string k_bindpose_animation_name = "bindpose";
    ui32 frame_3d_data::get_num_bones() const { return static_cast<ui32>(m_positions.size()); }
    ui64 frame_3d_data::get_revision() const { return m_revision; }
    frame_3d_data& frame_3d_data::operator=(const frame_3d_data& other)
    {
        if(this!=&other)
        {
            m_rotations=other.m_rotations;m_positions=other.m_positions;m_scales=other.m_scales;
            ++m_revision;
        }
        return *this;
    }
    std::shared_ptr<const animation_3d_rig> sequence_3d_transfering_data::get_rig() const { return m_rig; }
    std::shared_ptr<const animation_3d_rig> animation_3d_sequence::get_rig() const { return is_loaded()?m_sequence_data->get_rig():nullptr; }
    
    frame_3d_data::frame_3d_data(const std::vector<glm::quat>& rotations,
                                 const std::vector<glm::vec3>& positions,
                                 const std::vector<glm::vec3>& scales) :
    m_rotations(rotations),
    m_positions(positions),
    m_scales(scales)
    {
        
    }
    
    frame_3d_data::frame_3d_data(i32 num_bones)
    {
        m_positions.resize(num_bones);
        m_rotations.resize(num_bones);
        m_scales.resize(num_bones);
    }
    
    frame_3d_data::~frame_3d_data()
    {
        m_rotations.clear();
        m_positions.clear();
        m_scales.clear();
    }
    
    const glm::quat& frame_3d_data::get_rotation(ui32 index) const
    {
        assert(m_rotations.size() > index);
        return m_rotations.at(index);
    }
    
    const glm::vec3& frame_3d_data::get_position(ui32 index) const
    {
        assert(m_positions.size() > index);
        return m_positions.at(index);
    }
    
    const glm::vec3& frame_3d_data::get_scale(ui32 index) const
    {
        assert(m_scales.size() > index);
        return m_scales.at(index);
    }
    
    void frame_3d_data::update_position(ui32 index, const glm::vec3& position)
    {
        assert(m_positions.size() > index);
        m_positions[index] = position;
        ++m_revision;
    }
    
    void frame_3d_data::update_rotation(ui32 index, const glm::quat& rotation)
    {
        assert(m_rotations.size() > index);
        m_rotations[index] = rotation;
        ++m_revision;
    }
    
    void frame_3d_data::update_scale(ui32 index, const glm::vec3& scale)
    {
        assert(m_scales.size() > index);
        m_scales[index] = scale;
        ++m_revision;
    }
    
    sequence_3d_transfering_data::sequence_3d_transfering_data(const std::string& animation_name,
                                                               i32 animation_fps,
                                                               const std::vector<frame_3d_data_shared_ptr>& frames,
                                                               const std::shared_ptr<const animation_3d_rig>& rig) :
    m_animation_name(animation_name),
    m_animation_fps(animation_fps),
    m_frames(frames),
    m_rig(rig)
    {
        m_type = e_resource_transfering_data_type_sequence;
    }
    
    sequence_3d_transfering_data::~sequence_3d_transfering_data()
    {
        m_frames.clear();
    }
    
    i32 sequence_3d_transfering_data::get_num_frames() const
    {
        return static_cast<i32>(m_frames.size());
    }
    
    i32 sequence_3d_transfering_data::get_animation_fps() const
    {
        return m_animation_fps;
    }
    
    const std::string sequence_3d_transfering_data::get_animation_name() const
    {
        return m_animation_name;
    }
    
    frame_3d_data_shared_ptr sequence_3d_transfering_data::get_frame(i32 index) const
    {
        assert(m_frames.size() > index);
        if(index >= static_cast<i32>(m_frames.size()))
        {
            index = static_cast<i32>(m_frames.size()) - 1;
        }
        return m_frames.at(index);
    }
    
    animation_3d_sequence::animation_3d_sequence(const std::string& guid) :
    resource(e_resource_type_animation_sequence, guid),
    m_sequence_data(nullptr)
    {
        
    }
    
    animation_3d_sequence_shared_ptr animation_3d_sequence::construct(const std::string& guid,
                                                                      const sequence_3d_transfering_data_shared_ptr& data)
    {
        animation_3d_sequence_shared_ptr animation_sequence = std::make_shared<animation_3d_sequence>(guid);
        animation_sequence->on_transfering_data_serialized(data);
        animation_sequence->on_transfering_data_commited(data);
        return animation_sequence;
    }
    
    animation_3d_sequence::~animation_3d_sequence(void)
    {
        
    }
    
    void animation_3d_sequence::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_sequence:
            {
                m_sequence_data = std::static_pointer_cast<sequence_3d_transfering_data>(data);
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
    
    void animation_3d_sequence::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data)
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
    
    i32 animation_3d_sequence::get_num_frames() const
    {
        return resource::is_loaded() ? m_sequence_data->get_num_frames() : 0;
    }
    
    i32 animation_3d_sequence::get_animation_fps() const
    {
        return resource::is_loaded() ? m_sequence_data->get_animation_fps() : 0;
    }
    
    const std::string animation_3d_sequence::get_animation_name() const
    {
        return resource::is_loaded() ? m_sequence_data->get_animation_name() : "";
    }
    
    frame_3d_data_shared_ptr animation_3d_sequence::get_frame(i32 index) const
    {
        return resource::is_loaded() ? m_sequence_data->get_frame(index) : nullptr;
    }
}
