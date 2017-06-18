//
//  map.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_2d.h"
#include "ns_declarations.h"
#include "input_context.h"

namespace game
{
    class level : public gb::game_object_2d
    {
    public:
        
        enum e_level_layer
        {
            e_level_layer_default = 0,
            e_level_layer_bloodprints,
            e_level_layer_footprints,
            e_level_layer_bullets,
            e_level_layer_characters_down_statistic,
            e_level_layer_characters,
            e_level_layer_characters_top_statistic,
            e_level_layer_max
        };
        
        typedef std::function<void(const glm::vec2&)> on_touch_level_callback_t;
        
    private:
        
    protected:
        
        std::array<gb::game_object_2d_weak_ptr, e_level_layer_max> m_layers;
        path_map_shared_ptr m_path_map;
        
        glm::ivec2 m_level_size;
        glm::ivec2 m_cell_size;
        glm::ivec2 m_cells_count;
        
        gb::camera_2d_weak_ptr m_camera;
        
        on_touch_level_callback_t m_on_touch_level_callback;
        
        void on_touched(const gb::ces_entity_shared_ptr&, 
			const glm::vec2& point,
			const glm::ivec2& screen_size,
                        gb::e_input_source input_source,
                        gb::e_input_state input_state);
        
        
        
        
        
    public:
        
        level();
        ~level();
        
        void setup(const std::string& filename,
                   const gb::camera_2d_shared_ptr& camera,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                   const glm::ivec2& level_size,
                   const glm::ivec2& cell_size);
        
        std::property_ro<glm::ivec2> level_size;
        std::property_ro<glm::ivec2> cell_size;
        std::property_ro<path_map_shared_ptr> path_map;
        std::property_ro<std::array<gb::game_object_2d_weak_ptr, e_level_layer_max>> layers;
        
        gb::game_object_2d_shared_ptr get_layer(e_level_layer layer);
        
        void set_on_touch_level_callback(const on_touch_level_callback_t& callback);
    };
};
