//
//  level.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "level.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_extension.h"
#include "ces_box2d_body_component.h"
#include "ces_action_component.h"
#include "ces_bound_touch_component.h"
#include "ces_geometry_extension.h"
#include "path_map.h"
#include "camera_2d.h"
#include "glm_extensions.h"

#define k_camera_trashhold 64.f;

namespace game
{
    level::level() :
    m_map_size(0),
    m_cell_size(0),
    m_cells_count(0),
    m_path_map(nullptr),
    m_on_tap_on_map_callback(nullptr)
    {
        level_size.getter([=]() {
            return m_map_size;
        });
        
        cell_size.getter([=]() {
            return m_cell_size;
        });
        
        path_map.getter([=]() {
            return m_path_map;
        });
        
        layers.getter([=]() {
            return m_layers;
        });
    }
    
    level::~level()
    {
        
    }
    
    void level::setup(const std::string& filename,
                      const gb::camera_2d_shared_ptr& camera,
                      const gb::scene_graph_shared_ptr& scene_graph,
                      const gb::scene_fabricator_shared_ptr& scene_fabricator,
                      const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                      const glm::ivec2& level_size,
                      const glm::ivec2& cell_size)
    {
        m_camera = camera;
        
        m_map_size = level_size;
        m_cell_size = cell_size;
        assert(m_map_size.x % m_cell_size.x == 0 &&
               m_map_size.y % m_cell_size.y == 0);
        m_cells_count.x = m_map_size.x / m_cell_size.x;
        m_cells_count.y = m_map_size.y / m_cell_size.y;
        
        auto level = anim_fabricator->create_animated_sprite(filename, "level");
        level->position = glm::vec2(0.f, 0.f);
        level->goto_and_stop(0);
        level->is_shadow_caster = true;
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
        bound_touch_component->set_bounds(glm::vec4(0.f, 0.f, m_map_size.x, m_map_size.y));
        bound_touch_component->enable(gb::e_input_state_pressed, gb::e_input_source_mouse_left, true);
        bound_touch_component->enable(gb::e_input_state_released, gb::e_input_source_mouse_left, true);
        bound_touch_component->add_callback(gb::e_input_state_pressed, std::bind(&level::on_touched, this,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2,
                                                                                 std::placeholders::_3,
                                                                                 std::placeholders::_4));
        bound_touch_component->add_callback(gb::e_input_state_released, std::bind(&level::on_touched, this,
                                                                                  std::placeholders::_1,
                                                                                  std::placeholders::_2,
                                                                                  std::placeholders::_3,
                                                                                  std::placeholders::_4));
        level->add_component(bound_touch_component);
        level::add_child(level);
        
        scene_graph->enable_box2d_world(glm::vec2(0.f, 0.f),
                                        glm::vec2(m_map_size.x, m_map_size.y));
        
        gb::vbo::vertex_attribute_PTC vertices[4];
        vertices[0].m_position = glm::vec3(0.f, 0.f, 0.f);
        vertices[1].m_position = glm::vec3(m_map_size.x, 0.f, 0.f);
        vertices[2].m_position = glm::vec3(m_map_size.x, m_map_size.y, 0.f);
        vertices[3].m_position = glm::vec3(0.f, m_map_size.y, 0.f);
        
        gb::ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<gb::ces_convex_hull_component>();
        convex_hull_component->create(vertices, 4);
        level->add_component(convex_hull_component);
        
        gb::ces_shadow_component_shared_ptr shadow_component = std::make_shared<gb::ces_shadow_component>();
        level->add_component(shadow_component);
        
        const std::list<gb::ces_entity_shared_ptr>& map_tiles = level->children;
        ui32 map_tiles_count = level->children_count;
        
        m_path_map = std::make_shared<game::path_map>(m_cells_count, m_cell_size);
        
        for(i32 i = 0; i < m_cells_count.x; ++i)
        {
            for(i32 j = 0; j < m_cells_count.y; ++j)
            {
                m_path_map->set_path_passable(i, j, true);
            }
        }
        
        m_chunk_size = glm::vec2(512.f);
        m_chunk_cells = (m_map_size.x / m_chunk_size.x);
        m_chunk_rows = (m_map_size.y / m_chunk_size.y);
        m_chunks.resize(m_chunk_cells * m_chunk_rows);
        
        auto tile_it = map_tiles.begin();
        
        for (i32 i = 0; i < map_tiles_count; ++i)
        {
            std::stringstream str_stream;
            str_stream << "wall_" << i;
            auto weak_wall = level->get_named_part(str_stream.str());
            if (!weak_wall.expired())
            {
                auto wall = weak_wall.lock();
                auto pt1 = wall->get_named_part("pt_01");
                auto pt2 = wall->get_named_part("pt_02");
                auto pt3 = wall->get_named_part("pt_03");
                auto pt4 = wall->get_named_part("pt_04");
                
                if (pt1.lock() && pt2.lock() && pt3.lock() && pt4.lock())
                {
                    glm::vec2 point1 = pt1.lock()->position;
                    glm::vec2 point2 = pt2.lock()->position;
                    glm::vec2 point3 = pt3.lock()->position;
                    glm::vec2 point4 = pt4.lock()->position;
                    
                    gb::vbo::vertex_attribute_PTC vertices[4];
                    vertices[0].m_position = glm::vec3(point1.x, point1.y, 0.f);
                    vertices[1].m_position = glm::vec3(point2.x, point2.y, 0.f);
                    vertices[2].m_position = glm::vec3(point3.x, point3.y, 0.f);
                    vertices[3].m_position = glm::vec3(point4.x, point4.y, 0.f);
                    
                    glm::vec2 min_bound = glm::vec2(INT16_MAX);
                    glm::vec2 max_bound = glm::vec2(INT16_MIN);
                    
                    for(i32 i = 0; i < 4; ++i)
                    {
                        min_bound = glm::min(glm::vec2(vertices[i].m_position.x, vertices[i].m_position.y), min_bound);
                        max_bound = glm::max(glm::vec2(vertices[i].m_position.x, vertices[i].m_position.y), max_bound);
                    }
                    glm::vec2 size = max_bound - min_bound;
                    
                    glm::mat4 absolute_transformation = gb::ces_transformation_extension::get_absolute_transformation_in_ws(wall);
                    glm::vec2 absolute_position = glm::vec2(absolute_transformation[3][0], absolute_transformation[3][1]);
                    size = size - absolute_position;
                    
                    glm::ivec2 absolute_position_index;
                    absolute_position_index.x = absolute_position.x / m_cell_size.x;
                    absolute_position_index.y = absolute_position.y / m_cell_size.y;
                    m_path_map->set_path_passable(absolute_position_index.x, absolute_position_index.y, false);
                    
                    gb::ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<gb::ces_convex_hull_component>();
                    convex_hull_component->create(vertices, 4);
                    wall->add_component(convex_hull_component);
                    
                    gb::ces_shadow_component_shared_ptr shadow_component = std::make_shared<gb::ces_shadow_component>();
                    wall->add_component(shadow_component);
                    
                    wall->is_luminous = true;
                    
                    std::vector<b2Vec2> box2d_vertices;
                    box2d_vertices.push_back(b2Vec2(0.f, 0.f));
                    box2d_vertices.push_back(b2Vec2(m_cell_size.x, 0.f));
                    box2d_vertices.push_back(b2Vec2(m_cell_size.x, m_cell_size.y));
                    box2d_vertices.push_back(b2Vec2(0.f, m_cell_size.y));
                    
                    scene_graph->apply_box2d_physics(wall, b2BodyType::b2_staticBody, [box2d_vertices](gb::ces_box2d_body_component_const_shared_ptr component) {
                        component->shape = gb::ces_box2d_body_component::custom_geometry_convex;
                        component->set_custom_vertices(box2d_vertices);
                    });
                    
                    pt1.lock()->remove_from_parent();
                    pt2.lock()->remove_from_parent();
                    pt3.lock()->remove_from_parent();
                    pt4.lock()->remove_from_parent();
                }
            }
            
            auto tile = (*tile_it);
            auto transform_component = tile->get_component<gb::ces_transformation_2d_component>();
            glm::vec2 position = transform_component->get_position();
            position.x += m_cell_size.x * .5f;
            position.y += m_cell_size.y * .5f;
            
            i32 cels_index = position.x / m_chunk_size.x;
            i32 rows_index = position.y / m_chunk_size.y;
            
            m_chunks[cels_index + rows_index * m_chunk_cells].push_back(tile);
            tile_it++;
        }
        m_path_map->update();
        
        m_layers[e_level_layer_default] = level;
        
        gb::game_object_2d_shared_ptr layer = std::make_shared<gb::game_object_2d>();
        layer->tag = "layer_bloodprints";
        layer->z_order = 16.f;
        m_layers[e_level_layer_bloodprints] = layer;
        level::add_child(m_layers[e_level_layer_bloodprints].lock());
        
        layer = std::make_shared<gb::game_object_2d>();
        layer->tag = "layer_footprints";
        layer->z_order = 32.f;
        m_layers[e_level_layer_footprints] = layer;
        level::add_child(m_layers[e_level_layer_footprints].lock());
        
        layer = std::make_shared<gb::game_object_2d>();
        layer->tag = "layer_bullets";
        layer->z_order = 48.f;
        m_layers[e_level_layer_hit_bounds] = layer;
        level::add_child(m_layers[e_level_layer_hit_bounds].lock());
        
        layer = std::make_shared<gb::game_object_2d>();
        layer->tag = "layer_characters_down_statistic";
        layer->z_order = 64.f;
        m_layers[e_level_layer_characters_down_statistic] = layer;
        level::add_child(m_layers[e_level_layer_characters_down_statistic].lock());
        
        layer = std::make_shared<gb::game_object_2d>();
        layer->tag = "layer_characters";
        layer->z_order = 80.f;
        m_layers[e_level_layer_characters] = layer;
        level::add_child(m_layers[e_level_layer_characters].lock());
        
        layer = std::make_shared<gb::game_object_2d>();
        layer->tag = "layer_characters_top_statistic";
        layer->z_order = 96.f;
        m_layers[e_level_layer_characters_top_statistic] = layer;
        level::add_child(m_layers[e_level_layer_characters_top_statistic].lock());
        
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&level::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        level::add_component(action_component);
    }
    
    void level::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        m_camera_2d_bounds = m_camera.lock()->bound;
        m_camera_2d_bounds.x -= k_camera_trashhold;
        m_camera_2d_bounds.y -= k_camera_trashhold;
        m_camera_2d_bounds.z += k_camera_trashhold;
        m_camera_2d_bounds.w += k_camera_trashhold;
        
        glm::vec4 chunk_bounds;
        
        for(i32 i = 0; i < m_chunk_cells; ++i)
        {
            for(i32 j = 0; j < m_chunk_rows; ++j)
            {
                chunk_bounds.x = i * m_chunk_size.x;
                chunk_bounds.y = j * m_chunk_size.y;
                chunk_bounds.z = chunk_bounds.x + m_chunk_size.x;
                chunk_bounds.w = chunk_bounds.y + m_chunk_size.y;
                
                bool is_visible = glm::intersect(m_camera_2d_bounds, chunk_bounds);
                for(const auto& tile : m_chunks[i + j * m_chunk_cells])
                {
                    tile.lock()->visible = is_visible;
                }
            }
        }
    }
    
    gb::game_object_2d_shared_ptr level::get_layer(e_level_layer layer)
    {
        return m_layers[layer].lock();
    }
    
    void level::on_touched(const gb::ces_entity_shared_ptr&,
                           const glm::vec2& touch_point,
                           gb::e_input_source input_source,
                           gb::e_input_state input_state)
    {
        if(m_on_tap_on_map_callback && input_state == gb::e_input_state::e_input_state_released)
        {
            glm::ivec2 viewport_size = m_camera.lock()->viewport_size;
            glm::vec2 camera_pivot = m_camera.lock()->pivot;
            glm::vec2 position = m_camera.lock()->get_position();
            glm::vec2 offset = position + glm::vec2(viewport_size.x * camera_pivot.x,
				viewport_size.y * camera_pivot.y);
			glm::vec2 point_in_camera_space = touch_point - offset;
            m_on_tap_on_map_callback(point_in_camera_space);
        }
    }
    
    void level::set_on_tap_on_map_callback(const on_tap_on_map_callback_t& callback)
    {
        m_on_tap_on_map_callback = callback;
    }
}
