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
#include "ces_bound_touch_component.h"
#include "path_map.h"
#include "camera_2d.h"

namespace game
{
    level::level() :
    m_level_size(0),
    m_cell_size(0),
    m_cells_count(0),
    m_path_map(nullptr),
    m_on_touch_level_callback(nullptr)
    {
        level_size.getter([=]() {
            return m_level_size;
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
        
        m_level_size = level_size;
        m_cell_size = cell_size;
        assert(m_level_size.x % m_cell_size.x == 0 &&
               m_level_size.y % m_cell_size.y == 0);
        m_cells_count.x = m_level_size.x / m_cell_size.x;
        m_cells_count.y = m_level_size.y / m_cell_size.y;
        
        auto level = anim_fabricator->create_animated_sprite(filename, "level");
        level->position = glm::vec2(0.f, 0.f);
        level->goto_and_stop(0);
        level->is_shadow_caster = true;
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
        bound_touch_component->set_frame(glm::vec4(0.f, 0.f, m_level_size.x, m_level_size.y));
        bound_touch_component->enable(gb::e_input_state_pressed, gb::e_input_source_mouse_left, true);
        bound_touch_component->enable(gb::e_input_state_released, gb::e_input_source_mouse_left, true);
        bound_touch_component->add_callback(gb::e_input_state_pressed, std::bind(&level::on_touched, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        bound_touch_component->add_callback(gb::e_input_state_released, std::bind(&level::on_touched, this, std::placeholders::_1,
                                                                                  std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        level->add_component(bound_touch_component);
        level::add_child(level);
        
        scene_graph->enable_box2d_world(glm::vec2(0.f, 0.f),
                                        glm::vec2(m_level_size.x, m_level_size.y));
        
        gb::vbo::vertex_attribute_PTC vertices[4];
        vertices[0].m_position = glm::vec3(0.f, 0.f, 0.f);
        vertices[1].m_position = glm::vec3(m_level_size.x, 0.f, 0.f);
        vertices[2].m_position = glm::vec3(m_level_size.x, m_level_size.y, 0.f);
        vertices[3].m_position = glm::vec3(0.f, m_level_size.y, 0.f);
        
        gb::ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<gb::ces_convex_hull_component>();
        convex_hull_component->create(vertices, 4);
        level->add_component(convex_hull_component);
        
        gb::ces_shadow_component_shared_ptr shadow_component = std::make_shared<gb::ces_shadow_component>();
        level->add_component(shadow_component);
        
        std::list<gb::ces_entity_shared_ptr> level_children = level->children;
        size_t level_children_count = level_children.size();
        
        m_path_map = std::make_shared<game::path_map>(m_cells_count, m_cell_size);
        
        for(i32 i = 0; i < m_cells_count.x; ++i)
        {
            for(i32 j = 0; j < m_cells_count.y; ++j)
            {
                m_path_map->set_path_passable(i, j, true);
            }
        }
        
        for (i32 i = 0; i < level_children_count; ++i)
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
                    
                    pt1.lock()->visible = false;
                    pt2.lock()->visible = false;
                    pt3.lock()->visible = false;
                    pt4.lock()->visible = false;
                    
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
                }
            }
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
        m_layers[e_level_layer_bullets] = layer;
        level::add_child(m_layers[e_level_layer_bullets].lock());
        
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
    }
    
    gb::game_object_2d_shared_ptr level::get_layer(e_level_layer layer)
    {
        return m_layers[layer].lock();
    }
    
    void level::on_touched(const gb::ces_entity_shared_ptr&, const glm::vec2& point,
                           gb::e_input_source input_source,
                           gb::e_input_state input_state)
    {
        if(m_on_touch_level_callback)
        {
            glm::ivec2 screen_size = m_camera.lock()->screen_size;
            glm::vec2 camera_pivot = m_camera.lock()->pivot;
            glm::vec2 position = m_camera.lock()->get_position();
            glm::vec2 offset = position + glm::vec2(screen_size.x * camera_pivot.x,
                                                    screen_size.y * camera_pivot.y);
            m_on_touch_level_callback(point - offset);
        }
    }
    
    void level::set_on_touch_level_callback(const on_touch_level_callback_t& callback)
    {
        m_on_touch_level_callback = callback;
    }
}
