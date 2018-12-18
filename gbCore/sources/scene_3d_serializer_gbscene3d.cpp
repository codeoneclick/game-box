//
//  scene_3d_serializer_gbscene3d.cpp
//  gbCore
//
//  Created by serhii.s on 12/17/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#include "scene_3d_serializer_gbscene3d.h"
#include "scene_3d.h"

namespace gb
{
    scene_3d_serializer_gbscene3d::scene_3d_serializer_gbscene3d(const std::string& filename,
                                                                 const resource_shared_ptr& resource) :
    resource_serializer(filename, resource),
    m_filename(filename)
    {
        
    }
    
    scene_3d_serializer_gbscene3d::~scene_3d_serializer_gbscene3d()
    {
        
    }
    
    void scene_3d_serializer_gbscene3d::serialize(const resource_transfering_data_shared_ptr& transfering_data)
    {
        assert(m_resource != nullptr);
        m_status = e_serializer_status_in_progress;
        
        std::shared_ptr<std::istream> filestream = resource_serializer::open_stream(m_filename, &m_status);
        
        i32 num_scene_objects = 0;
        filestream->read((char*)&num_scene_objects, sizeof(i32));
        
        std::vector<scene_object_3d_data_shared_ptr> scene_objects;
        scene_objects.resize(num_scene_objects);
        
        for (i32 i = 0; i < num_scene_objects; ++i)
        {
            i32 name_length;
            std::string name;
            i32 parent_name_length;
            std::string parent_name;
           
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;
            
            filestream->read((char*)&name_length, sizeof(i32));
            if (name_length > 0)
            {
                char* string_buffer = new char[name_length];
                filestream->read(string_buffer, sizeof(char) * name_length);
                name.append(string_buffer);
                delete [] string_buffer;
            }
            
            filestream->read((char*)&parent_name_length, sizeof(i32));
            if (parent_name_length > 0)
            {
                char* string_buffer = new char[parent_name_length];
                filestream->read(string_buffer, sizeof(char) * parent_name_length);
                parent_name.append(string_buffer);
                delete [] string_buffer;
            }

            filestream->read((char*)&position, sizeof(glm::vec3));
            filestream->read((char*)&rotation, sizeof(glm::vec3));
            filestream->read((char*)&scale, sizeof(glm::vec3));
            
            scene_objects[i] = std::make_shared<scene_object_3d_data>(name,
                                                                      parent_name,
                                                                      position,
                                                                      rotation,
                                                                      scale);
        }
        
        const auto scene_transfering_data = std::make_shared<scene_3d_transfering_data>(scene_objects);
        
        resource_serializer::close_stream(filestream);
        m_status = e_serializer_status_success;
        resource_serializer::on_transfering_data_serialized(scene_transfering_data);
    }
}
