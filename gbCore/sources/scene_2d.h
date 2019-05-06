//
//  scene_2d.hpp
//  gbCore
//
//  Created by serhii.s on 2/13/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"

namespace gb
{
    enum e_scene_2d_object_type
    {
        e_scene_2d_object_type_line = 0,
        e_scene_2d_object_type_polygon,
        e_scene_2d_object_type_dummy
    };
    
    class scene_2d_object
    {
    private:
        
        e_scene_2d_object_type m_type;
        i32 m_id = -1;
        std::string m_name;
        glm::vec2 m_position;
        f32 m_rotation = 0.f;
        std::vector<glm::vec2> m_points;
        
    protected:
        
    public:
        
        scene_2d_object(e_scene_2d_object_type type, i32 id);
        
        void set_position(const glm::vec2& position);
        void set_points(const std::vector<glm::vec2>& points);
        
        void set_name(const std::string& name);
        std::string get_name() const;
        
        glm::vec2 get_position() const;
        const std::vector<glm::vec2>& get_points() const;
        
        void set_rotation(f32 rotation);
        f32 get_rotation() const;
        
        e_scene_2d_object_type get_type() const;
        
        i32 get_id() const;
    };
    
    class scene_2d_tile
    {
    private:
    
        i32 m_id = -1;
        i32 m_col = 0;
        i32 m_row = 0;
        
    protected:
        
    public:
        
        scene_2d_tile(i32 id, i32 col, i32 row);
        
        i32 get_id() const;
        
        i32 get_col() const;
        i32 get_row() const;
    };
    
    class scene_2d_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        i32 m_cols = 0;
        i32 m_rows = 0;
        
        glm::vec2 m_tile_size = glm::vec2(0.f);
        
        std::set<std::string> m_group_names;
        std::set<std::string> m_layer_names;
        
        std::map<std::string, std::vector<std::shared_ptr<scene_2d_object>>> m_object_groups;
        std::map<std::string, std::vector<std::vector<std::shared_ptr<scene_2d_tile>>>> m_tile_layers;
        
    public:
        
        scene_2d_transfering_data(i32 cols, i32 rows, const glm::vec2& tile_size);
        ~scene_2d_transfering_data();
    
        i32 get_cols() const;
        i32 get_rows() const;
        
        glm::vec2 get_tile_size() const;
        
        void add_object_to_group(const std::string& group_name, const std::shared_ptr<scene_2d_object>& object);
        void add_tile_to_layer(const std::string& layer_name, i32 col, i32 row, const std::shared_ptr<scene_2d_tile>& tile);
        
        std::set<std::string> get_group_names() const;
        std::set<std::string> get_layer_names() const;
        
        const std::vector<std::shared_ptr<scene_2d_object>>& get_objects(const std::string& group_name);
        const std::shared_ptr<scene_2d_tile> get_tile(const std::string& layer_name, i32 col, i32 row) const;
    };
    
    class scene_2d : public resource
    {
    private:
        
    protected:
        
        scene_2d_transfering_data_shared_ptr m_scene_2d_data;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        scene_2d(const std::string& guid);
        
        static scene_2d_shared_ptr construct(const std::string& guid,
                                             const scene_2d_transfering_data_shared_ptr& data);
        ~scene_2d();
        
        i32 get_cols() const;
        i32 get_rows() const;
        
        glm::vec2 get_tile_size() const;
        
        std::set<std::string> get_group_names() const;
        std::set<std::string> get_layer_names() const;
        
        const std::vector<std::shared_ptr<scene_2d_object>>& get_objects(const std::string& group_name);
        const std::shared_ptr<scene_2d_tile> get_tile(const std::string& layer_name, i32 col, i32 row) const;
    };
};


