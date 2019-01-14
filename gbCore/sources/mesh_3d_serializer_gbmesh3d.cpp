//
//  mesh_3d_serializer_gbmesh3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "mesh_3d_serializer_gbmesh3d.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "ibo.h"
#include "animation_3d_sequence.h"

namespace gb
{
    mesh_3d_serializer_gbmesh3d::mesh_3d_serializer_gbmesh3d(const std::string& filename,
                                                             const resource_shared_ptr& resource) :
    resource_serializer(filename, resource),
    m_filename(filename)
    {
        
    }
    
    mesh_3d_serializer_gbmesh3d::~mesh_3d_serializer_gbmesh3d()
    {
        
    }
    
    void mesh_3d_serializer_gbmesh3d::serialize(const resource_transfering_data_shared_ptr& transfering_data)
    {
        assert(m_resource != nullptr);
        m_status = e_serializer_status_in_progress;
        
        std::shared_ptr<std::istream> filestream = resource_serializer::open_stream(m_filename, &m_status);
        
        mesh_3d_shared_ptr mesh = std::static_pointer_cast<mesh_3d>(m_resource);
        
        ui32 num_vertices = 0;
        ui32 num_indices = 0;
        glm::vec3 max_bound(INT16_MIN);
        glm::vec3 min_bound(INT16_MAX);
        
        filestream->read((char*)&num_vertices, sizeof(ui32));
        filestream->read((char*)&num_indices, sizeof(ui32));
        
        mesh_3d_vertex_data* vertices = new mesh_3d_vertex_data[num_vertices];
        
        for(ui32 i = 0; i < num_vertices; ++i)
        {
            glm::vec3 position;
            filestream->read((char*)&position, sizeof(glm::vec3));
            glm::vec3 normal;
            filestream->read((char*)&normal, sizeof(glm::vec3));
            glm::vec3 tangent;
            filestream->read((char*)&tangent, sizeof(glm::vec3));
            glm::vec2 texcoord;
            filestream->read((char*)&texcoord, sizeof(glm::vec2));
            
            ui32 numWeights = 0;
            filestream->read((char*)&numWeights, sizeof(i32));
            
            for(ui32 j = 0; j < numWeights; ++j)
            {
                mesh_3d_bone_data bone;
                filestream->read((char*)&bone.m_id, sizeof(i32));
                filestream->read((char*)&bone.m_weigth, sizeof(f32));
                vertices[i].m_bones.push_back(bone);
            }
            
            vertices[i].m_position = position;
            vertices[i].m_texcoord = texcoord;
            vertices[i].m_normal = normal;
            vertices[i].m_tangent = tangent;
            
            if(vertices[i].m_position.x > max_bound.x)
            {
                max_bound.x = vertices[i].m_position.x;
            }
            if(vertices[i].m_position.y > max_bound.y)
            {
                max_bound.y = vertices[i].m_position.y;
            }
            if(vertices[i].m_position.z > max_bound.z)
            {
                max_bound.z = vertices[i].m_position.z;
            }
            if(vertices[i].m_position.x < min_bound.x)
            {
                min_bound.x = vertices[i].m_position.x;
            }
            if(vertices[i].m_position.y < min_bound.y)
            {
                min_bound.y = vertices[i].m_position.y;
            }
            if(vertices[i].m_position.z < min_bound.z)
            {
                min_bound.z = vertices[i].m_position.z;
            }
        }
        
        ui16* indices = new ui16[num_indices];
        
        for(ui32 i = 0; i < num_indices; ++i)
        {
            filestream->read((char*)&indices[i], sizeof(ui16));
        }
        
        for(ui32 i = 0; i < num_indices; i += 3)
        {
            ui16 index = indices[i + 1];
            indices[i + 1] = indices[i + 2];
            indices[i + 2] = index;
        }
        
        mesh_3d_transfering_data_shared_ptr mesh_transfering_data = std::make_shared<mesh_3d_transfering_data>(vertices, indices,
                                                                                                               num_vertices, num_indices,
                                                                                                               min_bound, max_bound);
        resource_serializer::on_transfering_data_serialized(mesh_transfering_data);
        
        ui32 num_bones;
        i32 id, parent_id;
        i32 bone_name_length;
        std::string bone_name;
        
        filestream->read((char*)&num_bones, sizeof(i32));
        assert(num_bones <= k_max_bones);
        skeleton_3d_transfering_data_shared_ptr skeleton_transfering_data = std::make_shared<skeleton_3d_transfering_data>(num_bones);
        
        for (ui32 i = 0; i < num_bones; ++i)
        {
            bone_name = "";
            
            filestream->read((char*)&id, sizeof(i32));
            filestream->read((char*)&parent_id, sizeof(i32));
            filestream->read((char*)&bone_name_length, sizeof(i32));
            
            if (bone_name_length > 0)
            {
                char* string_buffer = new char[bone_name_length];
                memset(string_buffer, NULL, bone_name_length * sizeof(char));
                filestream->read(string_buffer, bone_name_length * sizeof(char));
                bone_name.assign(string_buffer, bone_name_length);
                delete [] string_buffer;
            }
            
            skeleton_transfering_data->add_bone(id, parent_id, bone_name);
        }
        resource_serializer::on_transfering_data_serialized(skeleton_transfering_data);
        
        filestream->read((char*)&num_bones, sizeof(i32));
        assert(num_bones <= k_max_bones);
        i32 num_frames = 0;
        filestream->read((char*)&num_frames, sizeof(i32));
        
        std::vector<frame_3d_data_shared_ptr> frames;
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
            frames[i] = std::make_shared<frame_3d_data>(rotations,
                                                        positions,
                                                        scales);
        }
        
        sequence_3d_transfering_data_shared_ptr sequence_transfering_data = std::make_shared<sequence_3d_transfering_data>(k_bindpose_animation_name,
                                                                                                                           30,
                                                                                                                           frames);
        resource_serializer::on_transfering_data_serialized(sequence_transfering_data);
        
        resource_serializer::close_stream(filestream);
        m_status = e_serializer_status_success;
    }
}
