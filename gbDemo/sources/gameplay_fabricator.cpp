//
//  gameplay_fabricator.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "gameplay_fabricator.h"
#include "gameplay_configuration_accessor.h"
#include "resource_accessor.h"
#include "characters_3d_assembler.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "ces_level_controllers_component.h"
#include "ces_level_path_grid_component.h"
#include "ces_character_controllers_component.h"
#include "ces_character_statistic_component.h"
#include "ces_character_impact_component.h"
#include "ces_character_selector_component.h"
#include "ces_character_animation_component.h"
#include "ces_character_state_automat_component.h"
#include "ces_character_parts_component.h"
#include "ces_character_pathfinder_component.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"
#include "ces_box2d_body_component.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_bound_touch_3d_component.h"
#include "ces_ui_interaction_component.h"
#include "ces_ui_avatar_icon_component.h"
#include "vbo.h"
#include "path_map.h"
#include "shape_3d.h"
#include "shape_3d_configuration.h"
#include "light_source_2d.h"
#include "ces_transformation_extension.h"
#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"
#include "hit_bounds_controller.h"
#include "db_characters_table.h"
#include "db_character_quests_table.h"
#include "db_character_quest_tasks_table.h"
#include "database_entity.h"
#include "database_coordinator.h"
#include "ces_quest_giver_component.h"
#include "ces_quest_receiver_component.h"
#include "scene_fabricator.h"
#include "board_configuration.h"
#include "cell_configuration.h"
#include "element_configuration.h"
#include "scene_3d.h"
#include "scene_3d_loading_operation.h"
#include "fcl_script_loading_operation.h"
#include "fcl_script.h"
#include "ces_character_navigation_component.h"
#include "ces_car_model_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_car_input_component.h"
#include "ces_car_simulator_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_car_tire_trails_controller_component.h"
#include "ces_track_route_component.h"
#include "scene_2d.h"
#include "scene_2d_loading_operation.h"
#include "deferred_light_source_3d.h"
#include "particle_emitter.h"
#include "deferred_point_light_3d.h"
#include "deferred_spot_light_3d.h"
#include "label_3d.h"
#include "common.h"
#include "ces_particle_emitter_component.h"
#include "ces_car_replay_record_component.h"
#include "ces_car_replay_player_component.h"
#include "ces_sound_component.h"
#include "ces_car_fuzzy_logic_component.h"
#include "ces_car_sounds_set_component.h"
#include "ces_car_gear_component.h"

namespace game
{
    i32 gameplay_fabricator::g_character_guid = 0;
    
    gameplay_fabricator::gameplay_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator) :
    m_general_fabricator(general_fabricator)
    {
        m_gameplay_configuration_accessor = std::make_shared<gb::gameplay_configuration_accessor>();
        m_characters_3d_assembler = std::make_shared<characters_3d_assembler>();
        
        /*m_database_coordinator = std::make_shared<gb::db::database_coordinator>();
        m_database_coordinator->open("game.db");
        m_database_coordinator->register_table<db_characters_table>();
        m_database_coordinator->register_table<db_character_quests_table>();
        m_database_coordinator->register_table<db_character_quest_tasks_table>();*/
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_scene(const std::string& filename)
    {
        auto scene = gb::ces_entity::construct<gb::game_object_3d>();
        
        const auto general_fabricator = m_general_fabricator.lock();
        const auto resource_accessor = general_fabricator->get_resource_accessor();
        
        const auto scene_2d = resource_accessor->get_resource<gb::scene_2d, gb::scene_2d_loading_operation>(filename, true);
        
        const auto cols = scene_2d->get_cols();
        const auto rows = scene_2d->get_rows();
        
        for (i32 i = 0; i < cols; ++i)
        {
            for (i32 j = 0; j < rows; ++j)
            {
                const auto tile = scene_2d->get_tile("track", i, j);
                if (tile->get_id() == 0 ||
                    tile->get_id() == 1)
                {
                    const auto road_straight = general_fabricator->create_shape_3d("road_straight.xml");
                    scene->add_child(road_straight);
                    const auto road_straight_sidewalk = general_fabricator->create_shape_3d("road_straight_sidewalk.xml");
                    scene->add_child(road_straight_sidewalk);
                    
                    const auto position =  glm::vec3(i * 16.f + 8.f, 0.f, -j * 16.f - 8.f);
                    const auto rotation = glm::vec3(0.f, tile->get_id() == 1 ? 0.f : 90.f, 0.f);
                    
                    road_straight->position = position;
                    road_straight->rotation = rotation;
                   
                    road_straight_sidewalk->position = position;
                    road_straight_sidewalk->rotation = rotation;
                }
                
                if (tile->get_id() == 7 ||
                    tile->get_id() == 8 ||
                    tile->get_id() == 14 ||
                    tile->get_id() == 15)
                {
                    const auto road_corner = general_fabricator->create_shape_3d("road_corner.xml");
                    scene->add_child(road_corner);
                    
                    const auto road_corner_sidewalk = general_fabricator->create_shape_3d("road_corner_sidewalk.xml");
                    scene->add_child(road_corner_sidewalk);
                    
                    const auto position = glm::vec3(i * 16.f + 8.f, 0.f, -j * 16.f - 8.f);
                    auto rotation = glm::vec3(0.f);
                    
                    if (tile->get_id() == 7)
                    {
                        rotation = glm::vec3(0.f, 270.f, 0.f);
                    }
                    if (tile->get_id() == 8)
                    {
                        rotation = glm::vec3(0.f, 0.f, 0.f);
                    }
                    if (tile->get_id() == 14)
                    {
                        rotation = glm::vec3(0.f, 180.f, 0.f);
                    }
                    if (tile->get_id() == 15)
                    {
                        rotation = glm::vec3(0.f, 90.f, 0.f);
                    }
                    
                    road_corner->position = position;
                    road_corner->rotation = rotation;
                    
                    road_corner_sidewalk->position = position;
                    road_corner_sidewalk->rotation = rotation;
                }
                
                if (tile->get_id() == 2 ||
                    tile->get_id() == 3 ||
                    tile->get_id() == 9 ||
                    tile->get_id() == 10)
                {
                    const auto road_corner = general_fabricator->create_shape_3d("road_corner_t.xml");
                    scene->add_child(road_corner);
                    
                    const auto road_corner_sidewalk = general_fabricator->create_shape_3d("road_corner_t_sidewalk.xml");
                    scene->add_child(road_corner_sidewalk);
                    
                    const auto position = glm::vec3(i * 16.f + 8.f, 0.f, -j * 16.f - 8.f);
                    auto rotation = glm::vec3(0.f);
                    
                    if (tile->get_id() == 2)
                    {
                        rotation = glm::vec3(0.f, 270.f, 0.f);
                    }
                    if (tile->get_id() == 3)
                    {
                        rotation = glm::vec3(0.f, 0.f, 0.f);
                    }
                    if (tile->get_id() == 9)
                    {
                        rotation = glm::vec3(0.f, 180.f, 0.f);
                    }
                    if (tile->get_id() == 10)
                    {
                        rotation = glm::vec3(0.f, 90.f, 0.f);
                    }
                    
                    road_corner->position = position;
                    road_corner->rotation = rotation;
                    
                    road_corner_sidewalk->position = position;
                    road_corner_sidewalk->rotation = rotation;
                }
                
                if (tile->get_id() == 21)
                {
                    const auto sidewalk = general_fabricator->create_shape_3d("sidewalk.xml");
                    scene->add_child(sidewalk);
                    auto position = glm::vec3(i * 16.f + 8.f, 0.f, -j * 16.f - 8.f);
                    sidewalk->position = position;
                }
                
                if (tile->get_id() == 16 ||
                    tile->get_id() == 17)
                {
                    const auto road = general_fabricator->create_shape_3d("roads_shared.xml",
                                                                          "path_park_straight.GB3DMESH");
                    scene->add_child(road);
                    auto position = glm::vec3(i * 16.f + 8.f, 0.f, -j * 16.f - 8.f);
                    auto rotation = glm::vec3(0.f, tile->get_id() == 16 ? 0.f : 90.f, 0.f);;
                    
                    road->position = position;
                    road->rotation = rotation;
                }
            }
        }
        
        const auto spawners = scene_2d->get_objects("spawners");
        const auto spawner = spawners.at(0);
        auto spawner_position = spawner->get_position();
        spawner_position.x /= scene_2d->get_tile_size().x;
        spawner_position.y /= scene_2d->get_tile_size().y;
        
        const auto walls = scene_2d->get_objects("walls");
        for (const auto wall : walls)
        {
            auto position = wall->get_position();
            position.x /= scene_2d->get_tile_size().x;
            position.y /= scene_2d->get_tile_size().y;
            position.x *= 16.f;
            position.y *= -16.f;
            
            const auto points = wall->get_points();
            
            f32 wall_tickness = .75f;
          
            auto from = glm::vec2(position);
            auto point_position = points.at(0);
            point_position.x /= scene_2d->get_tile_size().x;
            point_position.y /= scene_2d->get_tile_size().y;
            point_position.x *= 16.f;
            point_position.y *= -16.f;
            
            from.x += point_position.x;
            from.y += point_position.y;
            
            for (i32 i = 1; i <= points.size() - 1; i++)
            {
                auto to = glm::vec2(position);
                point_position = points.at(i);
                point_position.x /= scene_2d->get_tile_size().x;
                point_position.y /= scene_2d->get_tile_size().y;
                point_position.x *= 16.f;
                point_position.y *= -16.f;
                
                to.x += point_position.x;
                to.y += point_position.y;
                
                f32 half_size = wall_tickness / 2.f;
                f32 cx = (from.x + to.x) / 2.f;
                f32 cy = (from.y + to.y) / 2.f;
                f32 angle = atan2f(to.y - from.y, to.x - from.x);
                f32 mag = sqrtf((to.x - from.x) * (to.x - from.x) + (to.y - from.y) * (to.y - from.y));
                
                f32 xmin = cx - mag / 2.f;
                f32 ymin = cy - half_size;
                f32 xmax = cx + mag / 2.f;
                f32 ymax = cy + half_size;
                
                cx = (xmin + xmax) / 2.f;
                cy = (ymin + ymax) / 2.f;
                f32 hx = (xmax - xmin) / 2.f;
                f32 hy = (ymax - ymin) / 2.f;
                
                if (hx < 0.f)
                {
                    hx = -hx;
                }
                if (hy < 0.f)
                {
                    hy = -hy;
                }

                auto wall_object_3d = gb::ces_entity::construct<gb::game_object_3d>();
                wall_object_3d->position = glm::vec3(cx, 0.f, cy);
                
                auto box2d_body_component = std::make_shared<gb::ces_box2d_body_component>();
                box2d_body_component->set_deferred_box2d_component_setup(wall_object_3d, b2BodyType::b2_staticBody, [=](gb::ces_box2d_body_component_const_shared_ptr component) {
                    component->shape = gb::ces_box2d_body_component::box;
                    component->set_hx(hx),
                    component->set_hy(hy),
                    component->set_center(glm::vec2(0.f));
                    component->set_angle(angle);
                });
                box2d_body_component->set_custom_box2d_body_setup([=](gb::ces_box2d_body_component_const_shared_ptr component,
                                                                      b2Body* box2d_body,
                                                                      std::shared_ptr<b2Shape> box2d_shape) {
                    const auto fixture = box2d_body->CreateFixture(box2d_shape.get(), 1.f);
                    fixture->SetFriction(.02f);
                    fixture->SetRestitution(0.f);
                    
                    static short k_group_walls = 0x0003;
                    
                    static short k_category_player = 0x0001;
                    static short k_category_opponent = 0x0002;
                    static short k_category_walls = 0x0004;
                    
                    static short k_mask_walls = k_category_player | k_category_opponent;

                    b2Filter filter;
                    filter.groupIndex = k_group_walls;
                    filter.categoryBits = k_category_walls;
                    filter.maskBits = k_mask_walls;
                    fixture->SetFilterData(filter);
                });
                wall_object_3d->add_component(box2d_body_component);
                scene->add_child(wall_object_3d);
                from = to;
            }
        }
        
        const auto lights = scene_2d->get_objects("lights");
        for (const auto light : lights)
        {
            auto position = light->get_position();
            position.x /= scene_2d->get_tile_size().x;
            position.y /= scene_2d->get_tile_size().y;
            position.x *= 16.f;
            position.y *= -16.f;
            
            const auto light_source = general_fabricator->create_deferred_point_light_3d("omni_light_source.xml");
            scene->add_child(light_source);
            light_source->position = glm::vec3(position.x, light_source->ray_length * .5f, position.y);
            
            const auto light_pole = general_fabricator->create_shape_3d("light_pole.xml");
            scene->add_child(light_pole);
            light_pole->position = glm::vec3(position.x, 0.f, position.y);
        }
        
        const auto trees = scene_2d->get_objects("trees");
        for (const auto tree : trees)
        {
            auto position = tree->get_position();
            position.x /= scene_2d->get_tile_size().x;
            position.y /= scene_2d->get_tile_size().y;
            position.x *= 16.f;
            position.y *= -16.f;
            
            const auto tree_01 = general_fabricator->create_shape_3d("tree_01.xml");
            scene->add_child(tree_01);
            tree_01->position = glm::vec3(position.x, 0.f, position.y);
        }
        
        const auto buildings = scene_2d->get_objects("buildings");
        for (const auto building : buildings)
        {
            auto position = building->get_position();
            position.x /= scene_2d->get_tile_size().x;
            position.y /= scene_2d->get_tile_size().y;
            position.x *= 16.f;
            position.y *= -16.f;
            
            std::string custom_mesh_filename = building->get_name();
            custom_mesh_filename.append(".GB3DMESH");
            const auto building_object = general_fabricator->create_shape_3d("buildings_shared.xml", custom_mesh_filename);
            scene->add_child(building_object);
            building_object->position = glm::vec3(position.x, .1f, position.y);
            building_object->rotation = glm::vec3(0.f, building->get_rotation(), 0.f);
        }
        
        auto track_route_component = std::make_shared<ces_track_route_component>();
        scene->add_component(track_route_component);
        
        const auto routes = scene_2d->get_objects("route");
        for (const auto route : routes)
        {
            auto position = route->get_position();
            position.x /= scene_2d->get_tile_size().x;
            position.y /= scene_2d->get_tile_size().y;
            position.x *= 16.f;
            position.y *= -16.f;
            
            const auto points = route->get_points();
            
            for (ui32 i = 0; i < points.size(); ++i)
            {
                auto point_position = points.at(i);
                point_position.x /= scene_2d->get_tile_size().x;
                point_position.y /= scene_2d->get_tile_size().y;
                point_position.x *= 16.f;
                point_position.y *= -16.f;
                point_position += position;
                track_route_component->add_route_point(point_position);
            }
        }
        
        auto level_controllers_component = std::make_shared<ces_level_controllers_component>();
        scene->add_component(level_controllers_component);
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_3d_component>();
        bound_touch_component->set_min_bound(glm::vec3(-4096.f, 0.f, -4096.f));
        bound_touch_component->set_max_bound(glm::vec3(4096.f, 0.f, 4096.f));
        scene->add_component(bound_touch_component);
        
        /*const auto general_fabricator = m_general_fabricator.lock();
        const auto resource_accessor = general_fabricator->get_resource_accessor();
        const auto scene_resource = resource_accessor->get_resource<gb::scene_3d, gb::scene_3d_loading_operation>(filename, true);
        for (i32 i = 0; i < scene_resource->get_num_objects(); ++i)
        {
            const auto scene_object = scene_resource->get_scene_object(i);
            auto scene_object_name = scene_object->get_name();
            scene_object_name.resize(scene_object_name.length() - 4);
            scene_object_name.append(".xml");
            
            const auto position = scene_object->get_position();
            const auto rotation = scene_object->get_rotation();
            const auto scale = scene_object->get_scale();
            
            auto scene_object_instance = general_fabricator->create_shape_3d(scene_object_name);
            scene_object_instance->position = position;
            scene_object_instance->rotation = rotation;
            scene_object_instance->scale = scale;
            scene->add_child(scene_object_instance);
        }*/
        
        return scene;
        
        /*auto level_layers_component = std::make_shared<ces_level_layers_component>();
         level->add_component(level_layers_component);
         
         auto level_path_grid_component = std::make_shared<ces_level_path_grid_component>(glm::ivec2(level_configuration->get_level_width(), level_configuration->get_level_height()),
         glm::ivec2(level_configuration->get_level_cell_width(), level_configuration->get_level_cell_height()));
         level->add_component(level_path_grid_component);
         
         auto level_controllers_component = std::make_shared<ces_level_controllers_component>();
         level->add_component(level_controllers_component);
         
         auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
         bound_touch_component->set_bounds(glm::vec4(0.f, 0.f, level_configuration->get_level_width(), level_configuration->get_level_height()));
         level->add_component(bound_touch_component);
         
         auto terrain = m_anim_fabricator.lock()->create_animated_sprite(level_configuration->get_visual_configuration_filename(), "level");
         terrain->position = glm::vec2(0.f, 0.f);
         terrain->goto_and_stop(0);
         terrain->is_shadow_caster = true;
         
         terrain->tag = ces_level_layers_component::k_layer_default;
         level_layers_component->set_layer(terrain, game::ces_level_layers_component::e_level_layer_default);
         level->add_child(terrain);
         
         gb::game_object_2d_shared_ptr layer = gb::ces_entity::construct<gb::game_object_2d>();
         layer->tag = ces_level_layers_component::k_layer_bloodprints;
         level_layers_component->set_layer(layer, game::ces_level_layers_component::e_level_layer_bloodprints);
         level->add_child(layer);
         
         layer = gb::ces_entity::construct<gb::game_object_2d>();
         layer->tag = ces_level_layers_component::k_layer_footprints;
         level_layers_component->set_layer(layer, game::ces_level_layers_component::e_level_layer_footprints);
         level->add_child(layer);
         
         layer = gb::ces_entity::construct<gb::game_object_2d>();
         layer->tag = ces_level_layers_component::k_layer_hit_bounds;
         level_layers_component->set_layer(layer, game::ces_level_layers_component::e_level_layer_hit_bounds);
         level->add_child(layer);
         
         layer = gb::ces_entity::construct<gb::game_object_2d>();
         layer->tag = ces_level_layers_component::k_layer_characters_down_statistic;
         level_layers_component->set_layer(layer, game::ces_level_layers_component::e_level_layer_characters_down_statistic);
         level->add_child(layer);
         
         layer = gb::ces_entity::construct<gb::game_object_2d>();
         layer->tag = ces_level_layers_component::k_layer_characters;
         level_layers_component->set_layer(layer, game::ces_level_layers_component::e_level_layer_characters);
         level->add_child(layer);
         
         layer = gb::ces_entity::construct<gb::game_object_2d>();
         layer->tag = ces_level_layers_component::k_layer_characters_top_statistic;
         level_layers_component->set_layer(layer, game::ces_level_layers_component::e_level_layer_characters_top_statistic);
         level->add_child(layer);
         
         path_map_shared_ptr path_grid = level_path_grid_component->get_path_grid();
         std::vector<gb::ces_entity_shared_ptr> tiles = terrain->children;
         ui32 tiles_count = terrain->children_count;
         
         for (i32 i = 0; i < tiles_count; ++i)
         {
         std::stringstream str_stream;
         str_stream<< "wall_"<<i;
         auto tile_weak = terrain->get_named_part(str_stream.str());
         if (!tile_weak.expired())
         {
         auto tile = tile_weak.lock();
         auto pt1 = tile->get_named_part("pt_01");
         auto pt2 = tile->get_named_part("pt_02");
         auto pt3 = tile->get_named_part("pt_03");
         auto pt4 = tile->get_named_part("pt_04");
         
         if (!pt1.expired() && !pt2.expired() && !pt3.expired() && !pt4.expired())
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
         
         glm::mat4 absolute_transformation = gb::ces_transformation_extension::get_absolute_transformation_in_ws(tile);
         glm::vec2 absolute_position = glm::vec2(absolute_transformation[3][0], absolute_transformation[3][1]);
         size = size - absolute_position;
         
         glm::ivec2 absolute_position_index;
         absolute_position_index.x = absolute_position.x / level_configuration->get_level_cell_width();
         absolute_position_index.y = absolute_position.y / level_configuration->get_level_cell_height();
         path_grid->set_path_passable(absolute_position_index.x, absolute_position_index.y, false);
         
         auto convex_hull_component = std::make_shared<gb::ces_convex_hull_component>();
         convex_hull_component->create(vertices, 4);
         tile->add_component(convex_hull_component);
         
         auto shadow_component = std::make_shared<gb::ces_shadow_component>();
         tile->add_component(shadow_component);
         
         auto box2d_body_component = std::make_shared<gb::ces_box2d_body_component>();
         tile->add_component(box2d_body_component);
         
         std::vector<b2Vec2> box2d_vertices;
         box2d_vertices.push_back(b2Vec2(0.f, 0.f));
         box2d_vertices.push_back(b2Vec2(level_configuration->get_level_cell_width(), 0.f));
         box2d_vertices.push_back(b2Vec2(level_configuration->get_level_cell_width(), level_configuration->get_level_cell_height()));
         box2d_vertices.push_back(b2Vec2(0.f, level_configuration->get_level_cell_height()));
         
         box2d_body_component->set_deferred_box2d_apply(tile, b2BodyType::b2_staticBody, [box2d_vertices](gb::ces_box2d_body_component_const_shared_ptr component) {
         component->shape = gb::ces_box2d_body_component::custom_geometry_convex;
         component->set_custom_vertices(box2d_vertices);
         });
         
         pt1.lock()->remove_from_parent();
         pt2.lock()->remove_from_parent();
         pt3.lock()->remove_from_parent();
         pt4.lock()->remove_from_parent();
         }
         }
         }
         path_grid->update();
         return level;*/
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_character(const std::string& filename)
    {
        const auto character_configuration = std::static_pointer_cast<gb::character_configuration>(m_gameplay_configuration_accessor->get_character_configuration(filename));
        
        const auto character_body_entity = m_general_fabricator.lock()->create_shape_3d(character_configuration->get_main_3d_configuration_filename());
        character_body_entity->tag = ces_character_parts_component::parts::k_body;
        
        std::stringstream character_guid;
        character_guid<<filename<<g_character_guid++;
        gb::game_object_3d_shared_ptr character = gb::ces_entity::construct<gb::game_object_3d>();
        character->tag = character_guid.str();
        character->add_child(character_body_entity);
        
        const auto character_parts_component = std::make_shared<ces_character_parts_component>();
        character_parts_component->setup(character_body_entity, nullptr, nullptr);
        character->add_component(character_parts_component);
        
        auto character_controllers_component = std::make_shared<ces_character_controllers_component>();
        character->add_component(character_controllers_component);
        
        auto character_statistic_component = std::make_shared<ces_character_statistic_component>();
        character_statistic_component->set_base_parameters(character_configuration->get_hp(),
                                                           character_configuration->get_move_speed(),
                                                           character_configuration->get_attack_speed(),
                                                           character_configuration->get_damage(),
                                                           character_configuration->get_attack_distance(),
                                                           character_configuration->get_reviving_time());
        character->add_component(character_statistic_component);
        
        auto character_navigation_component = std::make_shared<ces_character_navigation_component>();
        character->add_component(character_navigation_component);
        
        /*auto character_configuration = std::static_pointer_cast<gb::character_configuration>(m_gameplay_configuration_accessor->get_character_configuration(filename));
         auto character_main_3d_entity = m_general_fabricator.lock()->create_shape_3d(character_configuration->get_main_3d_configuration_filename());
         auto character_main_2d_entity = m_general_fabricator.lock()->create_sprite(character_configuration->get_main_2d_configuration_filename());
         auto character_avatar_3d_entity = m_general_fabricator.lock()->create_shape_3d(character_configuration->get_avatar_3d_configuration_filename());
         auto character_avatar_2d_entity = m_general_fabricator.lock()->create_sprite(character_configuration->get_avatar_2d_configuration_filename());
         
         m_characters_3d_assembler->assemble(character_main_2d_entity, character_main_3d_entity, glm::vec2(character_configuration->get_visual_size()), characters_3d_assembler::e_view_type_top);
         m_characters_3d_assembler->assemble(character_avatar_2d_entity, character_avatar_3d_entity, glm::vec2(character_configuration->get_visual_size()), characters_3d_assembler::e_view_type_front);
         character_avatar_2d_entity->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
         character_avatar_2d_entity->position = glm::vec2(40.f, 70.f);
         character_avatar_3d_entity->play_animation(ces_character_animation_component::animations::k_search_animation, true);
         
         std::stringstream character_guid;
         character_guid<<filename<<g_character_guid++;
         gb::game_object_2d_shared_ptr character = gb::ces_entity::construct<gb::game_object_2d>();
         character->tag = character_guid.str();
         character->add_child(character_main_3d_entity);
         character->add_child(character_main_2d_entity);
         
         auto light_source = m_general_fabricator.lock()->create_light_source_2d(character_configuration->get_light_source_configuration_filename());
         light_source->radius = 512.f;
         light_source->color = glm::vec4(0.f, 1.f, 0.f, 1.f);
         light_source->tag = ces_character_parts_component::parts::k_light_source_part;
         character->add_child(light_source);
         
         auto bounds = m_general_fabricator.lock()->create_sprite(character_configuration->get_bounds_configuration_filename());
         bounds->size = glm::vec2(character_configuration->get_bounds_size());
         bounds->pivot = glm::vec2(.5f, .5f);
         bounds->color = glm::u8vec4(0, 0, 0, 0);
         bounds->tag = ces_character_parts_component::parts::k_bounds_part;
         character->add_child(bounds);
         
         auto character_controllers_component = std::make_shared<ces_character_controllers_component>();
         
         auto information_bubble_controller = gb::ces_entity::construct<game::information_bubble_controller>(layers[ces_level_layers_component::e_level_layer_characters_top_statistic].lock(),
         m_general_fabricator.lock());
         character_controllers_component->information_bubble_controller = information_bubble_controller;
         character->add_child(information_bubble_controller);
         
         auto bloodprint_controller = gb::ces_entity::construct<game::bloodprint_controller>(layers[ces_level_layers_component::e_level_layer_bloodprints].lock(),
         m_general_fabricator.lock());
         character_controllers_component->bloodprint_controller = bloodprint_controller;
         character->add_child(bloodprint_controller);
         
         auto footprint_controller = gb::ces_entity::construct<game::footprint_controller>(layers[ces_level_layers_component::e_level_layer_footprints].lock(),
         m_general_fabricator.lock());
         character_controllers_component->footprint_controller = footprint_controller;
         character->add_child(footprint_controller);
         
         auto hit_bounds_controller = gb::ces_entity::construct<game::hit_bounds_controller>(layers[ces_level_layers_component::e_level_layer_hit_bounds].lock(),
         m_general_fabricator.lock());
         character_controllers_component->hit_bounds_controller = hit_bounds_controller;
         character->add_child(hit_bounds_controller);
         
         character->add_component(character_controllers_component);
         
         auto character_statistic_component = std::make_shared<ces_character_statistic_component>();
         character_statistic_component->set_base_parameters(character_configuration->get_hp(),
         character_configuration->get_move_speed(),
         character_configuration->get_attack_speed(),
         character_configuration->get_damage(),
         character_configuration->get_attack_distance(),
         character_configuration->get_reviving_time());
         auto health_status_entity = m_general_fabricator.lock()->create_sprite("character.statistic.2d.xml");
         health_status_entity->tag = "character.statistic.2d.xml";
         health_status_entity->size = glm::vec2(96.f);
         health_status_entity->pivot = glm::vec2(.5f, .5f);
         health_status_entity->color = glm::u8vec4(0, 255, 0, 255);
         layers[ces_level_layers_component::e_level_layer_characters_down_statistic].lock()->add_child(health_status_entity);
         character_statistic_component->set_health_status_entity(health_status_entity);
         
         character->add_component(character_statistic_component);
         
         auto character_selector_component = std::make_shared<ces_character_selector_component>();
         character->add_component(character_selector_component);
         
         auto character_animation_component = std::make_shared<ces_character_animation_component>();
         character_animation_component->set_3d_entity_linkage(character_main_3d_entity);
         character->add_component(character_animation_component);
         
         auto character_state_automat_component = std::make_shared<ces_character_state_automat_component>();
         character->add_component(character_state_automat_component);
         
         auto character_parts_component = std::make_shared<ces_character_parts_component>();
         character_main_2d_entity->tag = ces_character_parts_component::parts::k_body_part;
         character_parts_component->setup(character_main_2d_entity, bounds, light_source);
         character->add_component(character_parts_component);
         
         auto character_pathfinder_component = std::make_shared<ces_character_pathfinder_component>();
         character->add_component(character_pathfinder_component);
         
         auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
         bound_touch_component->set_bounds(glm::vec4(-static_cast<f32>(character_configuration->get_bounds_size()) * .5f, -static_cast<f32>(character_configuration->get_bounds_size()) * .5f,
         static_cast<f32>(character_configuration->get_bounds_size()) * .5f, static_cast<f32>(character_configuration->get_bounds_size()) * .5f));
         character->add_component(bound_touch_component);
         
         auto box2d_body_component = std::make_shared<gb::ces_box2d_body_component>();
         box2d_body_component->set_deferred_box2d_apply(character, b2BodyType::b2_dynamicBody, [character_configuration](gb::ces_box2d_body_component_const_shared_ptr component) {
         component->shape = gb::ces_box2d_body_component::circle;
         component->set_radius(static_cast<f32>(character_configuration->get_bounds_size()) * .5f);
         });
         character->add_component(box2d_body_component);
         
         auto ui_avatar_icon_component = std::make_shared<ces_ui_avatar_icon_component>();
         ui_avatar_icon_component->set_avatar_3d_entity(character_avatar_3d_entity);
         ui_avatar_icon_component->set_avatar_2d_entity(character_avatar_2d_entity);
         character->add_component(ui_avatar_icon_component);
         
         return character;*/
        return character;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_main_character(const std::string& filename)
    {
        const auto character = gameplay_fabricator::create_character(filename);
        auto character_statistic_component = character->get_component<ces_character_statistic_component>();
        character_statistic_component->mode = ces_character_statistic_component::e_mode_player;
        /*auto character = gameplay_fabricator::create_character(filename, layers);
         auto character_statistic_component = character->get_component<ces_character_statistic_component>();
         character_statistic_component->mode = ces_character_statistic_component::e_mode_player;
         auto quest_receiver_component = std::make_shared<ces_quest_receiver_component>();
         quest_receiver_component->set_database_coordinator(m_database_coordinator);
         character->add_component(quest_receiver_component);
         return character;*/
        return character;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_mob(const std::string& filename)
    {
        /*auto mob_configuration = std::static_pointer_cast<gb::mob_configuration>(m_gameplay_configuration_accessor->get_mob_configuration(filename));
         auto character = gameplay_fabricator::create_character(mob_configuration->get_character_configuration_filename(), layers);
         auto character_statistic_component = character->get_component<ces_character_statistic_component>();
         character_statistic_component->set_chase_paramaters(mob_configuration->get_max_distance_to_chase(),
         mob_configuration->get_max_distance_away_from_spawner());
         character_statistic_component->set_spawn_position(glm::vec2(mob_configuration->get_spawner_position_x(),
         mob_configuration->get_spawner_position_y()),
         mob_configuration->get_max_distance_delta_to_spawn());
         character_statistic_component->mode = ces_character_statistic_component::e_mode_mob;
         character_statistic_component->set_character_class_id(mob_configuration->get_id());
         auto character_parts_component = character->get_component<ces_character_parts_component>();
         auto light_source = std::static_pointer_cast<gb::light_source_2d>(character_parts_component->get_light_source_part());
         light_source->color = glm::vec4(1.f, 0.f, 0.f, 1.f);
         return character;*/
        return nullptr;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_npc(const std::string& filename)
    {
        /*auto npc_configuration = std::static_pointer_cast<gb::npc_configuration>(m_gameplay_configuration_accessor->get_npc_configuration(filename));
         auto character = gameplay_fabricator::create_character(npc_configuration->get_character_configuration_filename(), layers);
         auto character_parts_component = character->get_component<ces_character_parts_component>();
         auto light_source = std::static_pointer_cast<gb::light_source_2d>(character_parts_component->get_light_source_part());
         light_source->color = glm::vec4(0.f, 0.f, 1.f, 1.f);
         auto character_statistic_component = character->get_component<ces_character_statistic_component>();
         character_statistic_component->set_spawn_position(glm::vec2(npc_configuration->get_spawner_position_x(),
         npc_configuration->get_spawner_position_y()));
         character_statistic_component->mode = ces_character_statistic_component::e_mode_npc;
         character_statistic_component->set_character_class_id(npc_configuration->get_id());
         auto quest_giver_component = std::make_shared<ces_quest_giver_component>();
         for(auto configuration : npc_configuration->get_quest_configurations())
         {
         auto quest_configuration = std::static_pointer_cast<gb::quest_configuration>(configuration);
         std::vector<std::shared_ptr<gb::quest_task_configuration>> quest_tasks_configurations;
         for(auto configuration : quest_configuration->get_quest_task_configurations())
         {
         auto quest_task_configuration = std::static_pointer_cast<gb::quest_task_configuration>(configuration);
         quest_tasks_configurations.push_back(quest_task_configuration);
         }
         std::vector<std::shared_ptr<gb::quest_dependency_configuration>> quest_dependencies_configurations;
         for(auto configuration : quest_configuration->get_quest_dependencies())
         {
         auto quest_dependency_configuration = std::static_pointer_cast<gb::quest_dependency_configuration>(configuration);
         quest_dependencies_configurations.push_back(quest_dependency_configuration);
         }
         quest_giver_component->add_quest(quest_configuration, quest_dependencies_configurations, quest_tasks_configurations);
         }
         character->add_component(quest_giver_component);
         
         return character;*/
        return nullptr;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_car(const std::string& filename)
    {
        const auto car_configuration = std::static_pointer_cast<gb::character_configuration>(m_gameplay_configuration_accessor->get_character_configuration(filename));
        
        const auto car_body = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_main_3d_configuration_filename());
        car_body->tag = ces_character_parts_component::parts::k_body;
        
        std::stringstream car_guid;
        car_guid<<filename<<g_character_guid++;
        const auto car = gb::ces_entity::construct<gb::game_object_3d>();
        car->tag = car_guid.str();
        car->add_child(car_body);
        
        const auto car_tire_fl = m_general_fabricator.lock()->create_shape_3d("car_tire_l.xml");
        car_tire_fl->position = glm::vec3(.8f, .32f, 1.f);
        car->add_child(car_tire_fl);
        
        const auto car_tire_fr = m_general_fabricator.lock()->create_shape_3d("car_tire_r.xml");
        car_tire_fr->position = glm::vec3(-.8f, .32f, 1.f);
        car->add_child(car_tire_fr);
        
        const auto car_tire_rl = m_general_fabricator.lock()->create_shape_3d("car_tire_l.xml");
        car_tire_rl->position = glm::vec3(.8f, .32f, -1.1f);
        car->add_child(car_tire_rl);
        
        const auto car_tire_rr = m_general_fabricator.lock()->create_shape_3d("car_tire_r.xml");
        car_tire_rr->position = glm::vec3(-.8f, .32f, -1.1f);
        car->add_child(car_tire_rr);
        
        /*const auto exhaust_particle_emitter_left = m_general_fabricator.lock()->create_particle_emitter("exhaust.particle.emitter.xml");
        exhaust_particle_emitter_left->position = glm::vec3(-.7f, .5f, -1.7f);
        car->add_child(exhaust_particle_emitter_left);
        exhaust_particle_emitter_left->get_component<gb::ces_particle_emitter_component>()->set_enabled(true);
        
        const auto exhaust_particle_emitter_right = m_general_fabricator.lock()->create_particle_emitter("exhaust.particle.emitter.xml");
        exhaust_particle_emitter_right->position = glm::vec3(.7f, .5f, -1.7f);
        car->add_child(exhaust_particle_emitter_right);
        exhaust_particle_emitter_right->get_component<gb::ces_particle_emitter_component>()->set_enabled(true);*/
        
        const auto particle_emitter_smoke_01 = m_general_fabricator.lock()->create_particle_emitter("particle.emitter.smoke.xml");
        particle_emitter_smoke_01->position = glm::vec3(-.7f, .5f, -1.5f);
        car->add_child(particle_emitter_smoke_01);
        
        const auto particle_emitter_smoke_02 = m_general_fabricator.lock()->create_particle_emitter("particle.emitter.smoke.xml");
        particle_emitter_smoke_02->position = glm::vec3(.7f, .5f, -1.5f);
        car->add_child(particle_emitter_smoke_02);
        
        const auto light_source_01 = m_general_fabricator.lock()->create_deferred_spot_light_3d("cone_light_source.xml");
        car->add_child(light_source_01);
        light_source_01->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        light_source_01->position = glm::vec3(.25f, .75f, .0f);
        light_source_01->rotation = glm::vec3(0.f, 10.f, 0.f);
        light_source_01->outer_cutoff_angle = glm::cos(glm::radians(15.f));
        light_source_01->inner_cutoff_angle = glm::cos(glm::radians(10.f));
        light_source_01->scale = glm::vec3(8.f);

        const auto light_source_02 = m_general_fabricator.lock()->create_deferred_spot_light_3d("cone_light_source.xml");
        car->add_child(light_source_02);
        light_source_02->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        light_source_02->position = glm::vec3(-.25f, .75f, .0f);
        light_source_02->rotation = glm::vec3(0.f, -10.f, 0.f);
        light_source_02->outer_cutoff_angle = glm::cos(glm::radians(15.f));
        light_source_02->inner_cutoff_angle = glm::cos(glm::radians(10.f));
        light_source_02->scale = glm::vec3(8.f);
        
        const auto back_light_right = m_general_fabricator.lock()->create_deferred_point_light_3d("omni_light_source.xml");
        car->add_child(back_light_right);
        back_light_right->ray_length = 1.f;
        back_light_right->color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        back_light_right->position = glm::vec3(.5f, .75f, -1.85f);
        
        const auto back_light_left = m_general_fabricator.lock()->create_deferred_point_light_3d("omni_light_source.xml");
        car->add_child(back_light_left);
        back_light_left->ray_length = 1.f;
        back_light_left->color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        back_light_left->position = glm::vec3(-.5f, .75f, -1.85f);
        
        
        
        const auto car_parts_component = std::make_shared<ces_character_parts_component>();
        car_parts_component->add_part(car_body, ces_character_parts_component::parts::k_body);
        car_parts_component->add_part(car_tire_fl, ces_character_parts_component::parts::k_fl_tire);
        car_parts_component->add_part(car_tire_fr, ces_character_parts_component::parts::k_fr_tire);
        car_parts_component->add_part(car_tire_rl, ces_character_parts_component::parts::k_rl_tire);
        car_parts_component->add_part(car_tire_rr, ces_character_parts_component::parts::k_rr_tire);
        car_parts_component->add_part(light_source_01, ces_character_parts_component::parts::k_fl_light);
        car_parts_component->add_part(light_source_02, ces_character_parts_component::parts::k_fr_light);
        car_parts_component->add_part(back_light_left, ces_character_parts_component::parts::k_bl_light);
        car_parts_component->add_part(back_light_right, ces_character_parts_component::parts::k_br_light);
        car_parts_component->add_part(particle_emitter_smoke_01, ces_character_parts_component::parts::k_rl_tire_particles);
        car_parts_component->add_part(particle_emitter_smoke_02, ces_character_parts_component::parts::k_rr_tire_particles);
        car->add_component(car_parts_component);
        
        auto car_controllers_component = std::make_shared<ces_character_controllers_component>();
        car->add_component(car_controllers_component);
        
        auto car_statistic_component = std::make_shared<ces_character_statistic_component>();
        car_statistic_component->set_base_parameters(car_configuration->get_hp(),
                                                     car_configuration->get_move_speed(),
                                                     car_configuration->get_attack_speed(),
                                                     car_configuration->get_damage(),
                                                     car_configuration->get_attack_distance(),
                                                     car_configuration->get_reviving_time());
        car_statistic_component->mode = ces_character_statistic_component::e_mode_player;
        car->add_component(car_statistic_component);
        
        auto car_navigation_component = std::make_shared<ces_character_navigation_component>();
        car->add_component(car_navigation_component);
        
        auto car_model_component = std::make_shared<ces_car_model_component>();
        car->add_component(car_model_component);
        
        auto car_descriptor_component = std::make_shared<ces_car_descriptor_component>();
        car->add_component(car_descriptor_component);
        
        auto car_input_component = std::make_shared<ces_car_input_component>();
        car->add_component(car_input_component);
        
        auto car_simulator_component = std::make_shared<ces_car_simulator_component>();
        car->add_component(car_simulator_component);
        
        auto car_drift_state_component = std::make_shared<ces_car_drift_state_component>();
        car->add_component(car_drift_state_component);
        
        auto car_tire_trails_controller_component = std::make_shared<ces_car_tire_trails_controller_component>();
        car->add_component(car_tire_trails_controller_component);
        car_tire_trails_controller_component->set_parameters("tire_trail.xml", m_general_fabricator.lock(), car_tire_rl, car_tire_rr);
        
        auto box2d_body_component = std::make_shared<gb::ces_box2d_body_component>();
        box2d_body_component->set_deferred_box2d_component_setup(car, b2BodyType::b2_dynamicBody, [car_configuration](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(.8f);
        });
        box2d_body_component->set_custom_box2d_body_setup([=](gb::ces_box2d_body_component_const_shared_ptr component, b2Body* box2d_body, std::shared_ptr<b2Shape> box2d_shape) {
            const auto fixture = box2d_body->CreateFixture(box2d_shape.get(), car_model_component->get_density());
            fixture->SetFriction(car_model_component->get_friction());
            fixture->SetRestitution(car_model_component->get_restitution());
            
            b2MassData* box2d_mass_data = new b2MassData();
            box2d_body->GetMassData(box2d_mass_data);
            box2d_mass_data->center.Set(0, 0);
            box2d_body->SetMassData(box2d_mass_data);
            box2d_body->SetBullet(true);
            
            const auto car_input_component = car->get_component<ces_car_input_component>();
            
            static short k_category_walls = 0x0004;
            
            static short k_group_player = 0x0001;
            static short k_category_player = 0x0001;
            static short k_mask_player = k_category_walls;
            
            static short k_group_opponent = 0x0002;
            static short k_category_opponent = 0x0002;
            static short k_mask_opponent = k_category_walls;
            
            b2Filter filter;
            if (car_input_component)
            {
                filter.groupIndex = k_group_player;
                filter.categoryBits = k_category_player;
                filter.maskBits = k_mask_player;
            }
            else
            {
                filter.groupIndex = k_group_opponent;
                filter.categoryBits = k_category_opponent;
                filter.maskBits = k_mask_opponent;
            }
            fixture->SetFilterData(filter);
            
        });
        car->add_component(box2d_body_component);
        
        return car;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_player_car(const std::string& filename)
    {
        const auto car = create_car(filename);
        
        const auto name_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        name_label->text = "racer";
        name_label->font_size = 24;
        name_label->font_color = glm::u8vec4(255, 255, 255, 255);
        name_label->scale = glm::vec3(.05f);
        name_label->position = glm::vec3(name_label->get_content_size().x * .5f, 1.f, -3.f);
        name_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(name_label);
        
        const auto speed_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        speed_label->text = "speed";
        speed_label->font_size = 24;
        speed_label->font_color = glm::u8vec4(255, 255, 255, 255);
        speed_label->scale = glm::vec3(.05f);
        speed_label->position = glm::vec3(speed_label->get_content_size().x + 1.7f, 1.f, 2.f);
        speed_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(speed_label);
        
        const auto speed_value_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        speed_value_label->text = "0 km/h";
        speed_value_label->font_size = 24;
        speed_value_label->font_color = glm::u8vec4(255, 255, 255, 255);
        speed_value_label->scale = glm::vec3(.025f);
        speed_value_label->position = glm::vec3(speed_label->get_content_size().x + 1.7f, 1.f, .75f);
        speed_value_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(speed_value_label);
        
        const auto drift_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        drift_label->text = "drift";
        drift_label->font_size = 24;
        drift_label->font_color = glm::u8vec4(255, 255, 255, 255);
        drift_label->position = glm::vec3(speed_label->get_content_size().x + 1.7f, 1.f, 0.f);
        drift_label->scale = glm::vec3(.05f);
        drift_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(drift_label);
        
        const auto drift_value_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        drift_value_label->text = "00:00 sec";
        drift_value_label->font_size = 24;
        drift_value_label->font_color = glm::u8vec4(255, 255, 255, 255);
        drift_value_label->position = glm::vec3(speed_label->get_content_size().x + 1.7f, 1.f, -1.25f);
        drift_value_label->scale = glm::vec3(.025f);
        drift_value_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(drift_value_label);
        
        const auto car_parts_component = car->get_component<ces_character_parts_component>();
        car_parts_component->add_part(name_label, ces_character_parts_component::parts::k_ui_name_label);
        car_parts_component->add_part(speed_label, ces_character_parts_component::parts::k_ui_speed_label);
        car_parts_component->add_part(speed_value_label, ces_character_parts_component::parts::k_ui_speed_value_label);
        car_parts_component->add_part(drift_label, ces_character_parts_component::parts::k_ui_drift_label);
        car_parts_component->add_part(drift_value_label, ces_character_parts_component::parts::k_ui_drift_value_label);
        
        const auto car_replay_record_component = std::make_shared<ces_car_replay_record_component>();
        std::stringstream replay_record_guid;
        replay_record_guid<<"racer_"<<std::get_tick_count();
        car_replay_record_component->set_record_guid(replay_record_guid.str());
        car->add_component(car_replay_record_component);
        
        const auto car_gear_component = std::make_shared<ces_car_gear_component>();
        car->add_component(car_gear_component);
        
        const auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_drift, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_drift);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_drift, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_idle, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_idle);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_idle, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_off_low, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_off_low);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_low, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_off_mid, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_off_mid);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_mid, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_off_high, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_off_high);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_high, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_on_low, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_on_low);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_low, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_on_mid, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_on_mid);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_mid, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_on_high, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_on_high);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_high, 0.f);
        
        car->add_component(sound_component);
        
        const auto general_fabricator = m_general_fabricator.lock();
        const auto resource_accessor = general_fabricator->get_resource_accessor();
        
        const auto engine_load_fcl_script_id = resource_accessor->get_resource<gb::fcl_script, gb::fcl_script_loading_operation>("engine_load.fcl", true);
        
        const auto engine_idle_fcl_script_id = resource_accessor->get_resource<gb::fcl_script, gb::fcl_script_loading_operation>("engine_volume_idle.fcl", true);
        
        const auto engine_off_low_fcl_script_id = resource_accessor->get_resource<gb::fcl_script, gb::fcl_script_loading_operation>("engine_volume_off_low.fcl", true);
        
        const auto engine_off_mid_fcl_script_id = resource_accessor->get_resource<gb::fcl_script, gb::fcl_script_loading_operation>("engine_volume_off_mid.fcl", true);
        
        const auto engine_off_high_fcl_script_id = resource_accessor->get_resource<gb::fcl_script, gb::fcl_script_loading_operation>("engine_volume_off_high.fcl", true);
        
        const auto engine_on_low_fcl_script_id = resource_accessor->get_resource<gb::fcl_script, gb::fcl_script_loading_operation>("engine_volume_on_low.fcl", true);
        
        const auto engine_on_mid_fcl_script_id = resource_accessor->get_resource<gb::fcl_script, gb::fcl_script_loading_operation>("engine_volume_on_mid.fcl", true);
        
        const auto engine_on_high_fcl_script_id = resource_accessor->get_resource<gb::fcl_script, gb::fcl_script_loading_operation>("engine_volume_on_high.fcl", true);
        
        const auto car_fuzzy_logic_component = std::make_shared<ces_car_fuzzy_logic_component>();
        car_fuzzy_logic_component->set_parameters(engine_load_fcl_script_id->get_id(),
                                                  engine_idle_fcl_script_id->get_id(),
                                                  engine_off_low_fcl_script_id->get_id(),
                                                  engine_off_mid_fcl_script_id->get_id(),
                                                  engine_off_high_fcl_script_id->get_id(),
                                                  engine_on_low_fcl_script_id->get_id(),
                                                  engine_on_mid_fcl_script_id->get_id(),
                                                  engine_on_high_fcl_script_id->get_id());
        car->add_component(car_fuzzy_logic_component);
        
        const auto car_sounds_set_component = std::make_shared<ces_car_sounds_set_component>();
        car->add_component(car_sounds_set_component);
        
        return car;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_opponent_car(const std::string& filename)
    {
        const auto car = create_car(filename);
        car->remove_component(ces_car_input_component::class_guid());
        
        const auto name_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        name_label->text = "opponent";
        name_label->font_size = 24;
        name_label->font_color = glm::u8vec4(255, 255, 255, 255);
        name_label->position = glm::vec3(1.2f, 2.f, -3.f);
        name_label->scale = glm::vec3(.05f);
        name_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(name_label);
        const auto car_parts_component = car->get_component<ces_character_parts_component>();
        car_parts_component->add_part(name_label, ces_character_parts_component::parts::k_ui_name_label);
        
        const auto car_replay_player_component = std::make_shared<ces_car_replay_player_component>();
        car->add_component(car_replay_player_component);
        
        std::string replay_filename = documentspath();
        replay_filename.append("racer_66098269");
        replay_filename.append(".rep");
        std::ifstream instream;
        instream.open(replay_filename.c_str(), std::ios::in | std::ios::binary);
        if(instream.is_open())
        {
            ui32 records_count = 0;
            instream.read((char*)&records_count, sizeof(ui32));
            std::unordered_map<ui32, std::shared_ptr<ces_car_replay_player_component::replay_record_data>> record;
            for (ui32 i = 0; i < records_count; ++i)
            {
                ui32 tick = 0;
                instream.read((char*)&tick, sizeof(ui32));
                f32 linear_velocity_x = 0.f;
                instream.read((char*)&linear_velocity_x, sizeof(f32));
                f32 linear_velocity_y = 0.f;
                instream.read((char*)&linear_velocity_y, sizeof(f32));
                f32 angular_velocity = 0.f;
                instream.read((char*)&angular_velocity, sizeof(f32));
                
                const auto record_data = std::make_shared<ces_car_replay_player_component::replay_record_data>();
                record_data->set(glm::vec2(linear_velocity_x, linear_velocity_y), angular_velocity);
                record[tick] = record_data;
                
            }
            instream.close();
            car_replay_player_component->set_record(record);
        }
        
        return car;
    }
}
