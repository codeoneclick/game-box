//
//  scene_3d.cpp
//  gbCore
//
//  Created by serhii.s on 12/17/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#include "scene_3d.h"
#include "resource_status.h"

namespace gb
{
    scene_object_3d_data::scene_object_3d_data(const std::string& name,
                                               const std::string& parent_name,
                                               const glm::vec3& position,
                                               const glm::vec3& rotation,
                                               const glm::vec3& scale) :
    m_name(name),
    m_parent_name(parent_name),
    m_position(position),
    m_rotation(rotation),
    m_scale(scale)
    {
        
    }
    
    scene_object_3d_data::~scene_object_3d_data()
    {

    }
    
    const std::string& scene_object_3d_data::get_name() const
    {
        return m_name;
    }
    
    const std::string& scene_object_3d_data::get_parent_name() const
    {
        return m_parent_name;
    }
    
    const glm::vec3& scene_object_3d_data::get_position() const
    {
        return m_position;
    }
    
    const glm::vec3& scene_object_3d_data::get_rotation() const
    {
        return m_rotation;
    }
    
    const glm::vec3& scene_object_3d_data::get_scale() const
    {
        return m_scale;
    }
    
    scene_3d_transfering_data::scene_3d_transfering_data(const std::vector<scene_object_3d_data_shared_ptr>& scene_objects) :
    m_scene_objects(scene_objects)
    {
        m_type = e_resource_transfering_data_type_scene_3d;
    }
    
    scene_3d_transfering_data::~scene_3d_transfering_data()
    {
        m_scene_objects.clear();
    }
    
    i32 scene_3d_transfering_data::get_num_objects() const
    {
        return static_cast<i32>(m_scene_objects.size());
    }
    
    scene_object_3d_data_shared_ptr scene_3d_transfering_data::get_scene_object(i32 index) const
    {
        assert(m_scene_objects.size() > index);
        if(index >= static_cast<i32>(m_scene_objects.size()))
        {
            index = static_cast<i32>(m_scene_objects.size()) - 1;
        }
        return m_scene_objects.at(index);
    }
    
    scene_3d::scene_3d(const std::string& guid) :
    resource(e_resource_type_scene_3d, guid),
    m_scene_3d_data(nullptr)
    {
        
    }
    
    scene_3d_shared_ptr scene_3d::construct(const std::string& guid,
                                            const scene_3d_transfering_data_shared_ptr& data)
    {
        scene_3d_shared_ptr scene_3d_resource = std::make_shared<scene_3d>(guid);
        scene_3d_resource->on_transfering_data_serialized(data);
        scene_3d_resource->on_transfering_data_commited(data);
        return scene_3d_resource;
    }
    
    scene_3d::~scene_3d(void)
    {
        
    }
    
    void scene_3d::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_scene_3d:
            {
                m_scene_3d_data = std::static_pointer_cast<scene_3d_transfering_data>(data);
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
    
    void scene_3d::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_scene_3d:
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
    
    i32 scene_3d::get_num_objects() const
    {
        return resource::is_loaded() ? m_scene_3d_data->get_num_objects() : 0;
    }
    
    scene_object_3d_data_shared_ptr scene_3d::get_scene_object(i32 index) const
    {
        return resource::is_loaded() ? m_scene_3d_data->get_scene_object(index) : nullptr;
    }
}
