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
    
    scene_2d_object::scene_2d_object(e_scene_2d_object_type type, i32 id) :
    m_type(type),
    m_id(id)
    {
        
    }
    
    void scene_2d_object::set_position(const glm::vec2& position)
    {
        m_position = position;
    }
    
    void scene_2d_object::set_points(const std::vector<glm::vec2>& points)
    {
        m_points = points;
    }
    
    glm::vec2 scene_2d_object::get_position() const
    {
        return m_position;
    }
    
    const std::vector<glm::vec2>& scene_2d_object::get_points() const
    {
        return m_points;
    }
    
    e_scene_2d_object_type scene_2d_object::get_type() const
    {
        return m_type;
    }
    
    i32 scene_2d_object::get_id() const
    {
        return m_id;
    }
    
    scene_2d_tile::scene_2d_tile(i32 id, i32 col, i32 row) :
    m_id(id),
    m_col(col),
    m_row(row)
    {
        
    }
    
    i32 scene_2d_tile::get_id() const
    {
        return m_id;
    }
    
    i32 scene_2d_tile::get_col() const
    {
        return m_col;
    }
    
    i32 scene_2d_tile::get_row() const
    {
        return m_row;
    }
    
    scene_2d_transfering_data::scene_2d_transfering_data(i32 cols, i32 rows, const glm::vec2& tile_size) :
    m_cols(cols),
    m_rows(rows),
    m_tile_size(tile_size)
    {
        m_type = e_resource_transfering_data_type_scene_2d;
    }
    
    scene_2d_transfering_data::~scene_2d_transfering_data()
    {

    }
    
    i32 scene_2d_transfering_data::get_cols() const
    {
        return m_cols;
    }
    
    i32 scene_2d_transfering_data::get_rows() const
    {
        return m_rows;
    }
    
    glm::vec2 scene_2d_transfering_data::get_tile_size() const
    {
        return m_tile_size;
    }
    
    std::set<std::string> scene_2d_transfering_data::get_group_names() const
    {
        return m_group_names;
    }
    
    std::set<std::string> scene_2d_transfering_data::get_layer_names() const
    {
        return m_layer_names;
    }
    
    const std::vector<std::shared_ptr<scene_2d_object>>& scene_2d_transfering_data::get_objects(const std::string& group_name) const
    {
        const auto object_group_it = m_object_groups.find(group_name);
        if (object_group_it != m_object_groups.end())
        {
            return object_group_it->second;
        }
        return std::move(std::vector<std::shared_ptr<scene_2d_object>>());
    }
    
    const std::shared_ptr<scene_2d_tile> scene_2d_transfering_data::get_tile(const std::string& layer_name, i32 col, i32 row) const
    {
        std::shared_ptr<scene_2d_tile> tile = nullptr;
        const auto tile_layer_it = m_tile_layers.find(layer_name);
        if (tile_layer_it != m_tile_layers.end() && col >= 0 && col < m_cols && row >= 0 && row < m_rows)
        {
            tile = tile_layer_it->second.at(col).at(row);
        }
        return tile;
    }
    
    void scene_2d_transfering_data::add_object_to_group(const std::string& group_name, const std::shared_ptr<scene_2d_object>& object)
    {
        m_group_names.insert(group_name);
        m_object_groups[group_name].push_back(object);
    }
    
    void scene_2d_transfering_data::add_tile_to_layer(const std::string& layer_name, i32 col, i32 row, const std::shared_ptr<scene_2d_tile>& tile)
    {
        m_layer_names.insert(layer_name);
        const auto tile_layer_it = m_tile_layers.find(layer_name);
        if (tile_layer_it == m_tile_layers.end())
        {
            m_tile_layers[layer_name].resize(m_cols);
            for (i32 i = 0; i < m_cols; ++i)
            {
                m_tile_layers[layer_name].at(i).resize(m_rows, nullptr);
            }
        }
        m_tile_layers[layer_name][col][row] = tile;
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
    
    i32 scene_2d::get_cols() const
    {
        return resource::is_loaded() ? m_scene_2d_data->get_cols() : 0;
    }
    
    i32 scene_2d::get_rows() const
    {
         return resource::is_loaded() ? m_scene_2d_data->get_rows() : 0;
    }
    
    glm::vec2 scene_2d::get_tile_size() const
    {
        return m_scene_2d_data->get_tile_size();
    }
    
    std::set<std::string> scene_2d::get_group_names() const
    {
        return m_scene_2d_data->get_group_names();
    }
    
    std::set<std::string> scene_2d::get_layer_names() const
    {
        return m_scene_2d_data->get_layer_names();
    }
    
    const std::vector<std::shared_ptr<scene_2d_object>>& scene_2d::get_objects(const std::string& group_name) const
    {
         return m_scene_2d_data->get_objects(group_name);
    }
    
    const std::shared_ptr<scene_2d_tile> scene_2d::get_tile(const std::string& layer_name, i32 col, i32 row) const
    {
        return m_scene_2d_data->get_tile(layer_name, col, row);
    }
}
