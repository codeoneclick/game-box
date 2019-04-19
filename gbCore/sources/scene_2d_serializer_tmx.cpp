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
        gb::scene_2d_transfering_data_shared_ptr scene_2d_transfering_data = nullptr;
        
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
            scene_2d_transfering_data = std::make_shared<gb::scene_2d_transfering_data>(map->GetWidth(),
                                                                                        map->GetHeight(),
                                                                                        glm::vec2(map->GetTileWidth(),
                                                                                                  map->GetTileHeight()));
            
            const auto map_objects_groups = map->GetObjectGroups();
            for (auto map_objects_group : map_objects_groups)
            {
                const auto map_objects = map_objects_group->GetObjects();
                for (auto map_object : map_objects)
                {
                    std::shared_ptr<scene_2d_object> scene_2d_object = nullptr;
                    if (map_object->GetPolyline())
                    {
                        const auto line = map_object->GetPolyline();
                        scene_2d_object = std::make_shared<gb::scene_2d_object>(e_scene_2d_object_type_line, map_object->GetId());
                        std::vector<glm::vec2> points;
                        for (i32 i = 0; i < line->GetNumPoints(); ++i)
                        {
                            const auto point_tmx = line->GetPoint(i);
                            points.push_back(glm::vec2(point_tmx.x, point_tmx.y));
                        }
                        scene_2d_object->set_points(points);
                        scene_2d_object->set_position(glm::vec2(map_object->GetX(),
                                                                map_object->GetY()));
                        scene_2d_transfering_data->add_object_to_group(map_objects_group->GetName(), scene_2d_object);
                    }
                    else if (map_object->GetPolygon())
                    {
                        const auto polygon = map_object->GetPolygon();
                        scene_2d_object = std::make_shared<gb::scene_2d_object>(e_scene_2d_object_type_polygon, map_object->GetId());
                        std::vector<glm::vec2> points;
                        for (i32 i = 0; i < polygon->GetNumPoints(); ++i)
                        {
                            const auto point_tmx = polygon->GetPoint(i);
                            points.push_back(glm::vec2(point_tmx.x, point_tmx.y));
                        }
                        scene_2d_object->set_points(points);
                        scene_2d_object->set_position(glm::vec2(map_object->GetX(),
                                                                map_object->GetY()));
                        scene_2d_transfering_data->add_object_to_group(map_objects_group->GetName(), scene_2d_object);
                    }
                    else
                    {
                        scene_2d_object = std::make_shared<gb::scene_2d_object>(e_scene_2d_object_type_dummy, map_object->GetId());
                        scene_2d_object->set_position(glm::vec2(map_object->GetX(),
                                                                map_object->GetY()));
                        scene_2d_object->set_name(map_object->GetName());
                        scene_2d_object->set_rotation(map_object->GetRot());
                        scene_2d_transfering_data->add_object_to_group(map_objects_group->GetName(), scene_2d_object);
                    }
                }
            }
            const auto map_layers = map->GetTileLayers();
            for (auto map_layer : map_layers)
            {
                const auto cols = map->GetWidth();
                const auto rows = map->GetHeight();
                for (i32 i = 0; i < cols; ++i)
                {
                    for (i32 j = 0; j < rows; ++j)
                    {
                        const auto scene_2d_tile = std::make_shared<gb::scene_2d_tile>(map_layer->GetTileId(i, j), i, j);
                        scene_2d_transfering_data->add_tile_to_layer(map_layer->GetName(), i, j, scene_2d_tile);
                    }
                }
            }
        }
        
        resource_serializer::on_transfering_data_serialized(scene_2d_transfering_data);
    }
}
