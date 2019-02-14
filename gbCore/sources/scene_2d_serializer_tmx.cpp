//
//  map_serializer_tmx.cpp
//  gbCore
//
//  Created by serhii.s on 2/13/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "scene_2d_serializer_tmx.h"
#include "scene_2d.h"
#include "common.h"

namespace gb
{
    scene_2d_serializer_tmx::scene_2d_serializer_tmx(const std::string& filename,
                                                     const resource_shared_ptr& resource) :
    resource_serializer(filename, resource),
    m_filename(filename)
    {
        
    }
    
    scene_2d_serializer_tmx::~scene_2d_serializer_tmx()
    {
        
    }
    
    void scene_2d_serializer_tmx::serialize(const resource_transfering_data_shared_ptr& transfering_data)
    {
        assert(m_resource != nullptr);
        m_status = e_serializer_status_in_progress;
        
        std::shared_ptr<Tmx::Map> map = std::make_shared<Tmx::Map>();
        map->ParseFile(bundlepath().append(m_filename).c_str());
        
        if (map->HasError())
        {
            std::cout<<"map TMX error code: "<<map->GetErrorCode()<<std::endl;
            std::cout<<"map TMX error text: "<<map->GetErrorText()<<std::endl;
            
            m_status = e_serializer_status_failure;
        }
        else
        {
            m_status = e_serializer_status_success;
            
            const auto map_objects_groups = map->GetObjectGroups();
            for (auto map_objects_group : map_objects_groups)
            {
                std::cout<<"map TMX objects group name: "<<map_objects_group->GetName()<<std::endl;
                if (map_objects_group->GetName() == "walls")
                {
                    const auto map_objects = map_objects_group->GetObjects();
                    for (auto map_object : map_objects)
                    {
                        const auto polyline = map_object->GetPolyline();
                        std::cout<<"map TMX polyline count: "<<polyline->GetNumPoints()<<std::endl;
                    }
                }
            }
            const auto map_layers = map->GetTileLayers();
            for (auto map_layer : map_layers)
            {
                std::cout<<"map TMX layer name: "<<map_layer->GetName()<<std::endl;
                if (map_layer->GetName() == "track")
                {
                    const auto cols = map->GetWidth();
                    const auto rows = map->GetHeight();
                    for (i32 i = 0; i < cols; ++i)
                    {
                        for (i32 j = 0; j < rows; ++j)
                        {
                             std::cout<<"map TMX tile id: "<<map_layer->GetTileId(i, j)<<std::endl;
                        }
                    }
                }
            }
        }
        
        const auto scene_transfering_data = std::make_shared<scene_2d_transfering_data>(map);
        resource_serializer::on_transfering_data_serialized(scene_transfering_data);
    }
}
