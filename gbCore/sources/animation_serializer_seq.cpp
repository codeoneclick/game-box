//
//  animation_serializer_seq.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "animation_serializer_seq.h"
#include "sequence.h"

namespace gb
{
    animation_serializer_seq::animation_serializer_seq(const std::string& filename, const resource_shared_ptr& resource) :
    gb::resource_serializer(filename, resource),
    m_filename(filename)
    {
        
    }
    
    animation_serializer_seq::~animation_serializer_seq(void)
    {
        
    }
    
    void animation_serializer_seq::serialize(void)
    {
        assert(m_resource != nullptr);
        m_status = e_serializer_status_in_progress;
        
        std::shared_ptr<std::istream> filestream = resource_serializer::open_stream(m_filename, &m_status);
        
        i32 num_bones = 0;
        filestream->read((char*)&num_bones, sizeof(i32));
        i32 num_frames = 0;
        filestream->read((char*)&num_frames, sizeof(i32));
        
        std::vector<std::shared_ptr<frame_data>> frames;
        frames.resize(num_frames);
        
        for (ui32 i = 0; i < num_frames; ++i)
        {
            std::vector<glm::quat> rotations;
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> scales;
            
            for (i32 i = 0; i < num_bones; ++i)
            {
                glm::vec3 position;
                filestream->read((char*)&position, sizeof(glm::vec3));
                positions.push_back(glm::vec3(position));
                
                glm::quat rotation;
                filestream->read((char*)&rotation, sizeof(glm::quat));
                glm::quat _rotation = glm::quat(rotation.w, -rotation.x, -rotation.y, -rotation.z);
                rotations.push_back(_rotation);
                
                glm::vec3 scale;
                filestream->read((char*)&scale, sizeof(glm::vec3));
                scales.push_back(scale);
            }
            frames[i] = std::make_shared<frame_data>(rotations, positions, scales);
        }
        sequence_transfering_data_shared_ptr sequence_transfering_data = std::make_shared<gb::sequence_transfering_data>(m_filename, 30, frames);
        
        resource_serializer::close_stream(filestream);
        resource_serializer::on_transfering_data_serialized(sequence_transfering_data);
        
        m_status = e_serializer_status_success;
    }
}