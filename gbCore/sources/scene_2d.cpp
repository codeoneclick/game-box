//
//  scene_2d.cpp
//  gbCore
//
//  Created by serhii.s on 2/13/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "scene_2d.h"
#include "resource_status.h"

namespace gb
{
    scene_2d_transfering_data::scene_2d_transfering_data(const std::shared_ptr<Tmx::Map>& map) :
    m_map(map)
    {
        m_type = e_resource_transfering_data_type_scene_2d;
    }
    
    scene_2d_transfering_data::~scene_2d_transfering_data()
    {

    }
    
    std::shared_ptr<Tmx::Map> scene_2d_transfering_data::get_map() const
    {
        return m_map;
    }
    
    scene_2d::scene_2d(const std::string& guid) :
    resource(e_resource_type_scene_2d, guid),
    m_scene_2d_data(nullptr)
    {
        
    }
    
    scene_2d_shared_ptr scene_2d::construct(const std::string& guid,
                                            const scene_2d_transfering_data_shared_ptr& data)
    {
        scene_2d_shared_ptr scene_2d_resource = std::make_shared<scene_2d>(guid);
        scene_2d_resource->on_transfering_data_serialized(data);
        scene_2d_resource->on_transfering_data_commited(data);
        return scene_2d_resource;
    }
    
    scene_2d::~scene_2d()
    {
        
    }
    
    void scene_2d::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_scene_2d:
            {
                m_scene_2d_data = std::static_pointer_cast<scene_2d_transfering_data>(data);
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
    
    void scene_2d::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_scene_2d:
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
    
    std::shared_ptr<Tmx::Map> scene_2d::get_map() const
    {
        return resource::is_loaded() ? m_scene_2d_data->get_map() : nullptr;
    }
}
