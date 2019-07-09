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
#include "scene_fabricator.h"
#include "ces_car_statistic_component.h"
#include "ces_scene_state_automat_component.h"
#include "ces_car_parts_component.h"
#include "ces_box2d_body_component.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_bound_touch_3d_component.h"
#include "ces_ui_interaction_component.h"
#include "vbo.h"
#include "shape_3d.h"
#include "shape_3d_configuration.h"
#include "light_source_2d.h"
#include "ces_transformation_extension.h"
#include "database_entity.h"
#include "database_coordinator.h"
#include "scene_fabricator.h"
#include "scene_3d.h"
#include "scene_3d_loading_operation.h"
#include "fcl_script_loading_operation.h"
#include "fcl_script.h"
#include "ces_car_model_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_car_input_component.h"
#include "ces_car_simulator_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_car_tire_trails_controller_component.h"
#include "ces_level_route_component.h"
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
#include "ces_car_ai_input_component.h"
#include "ces_level_descriptor_component.h"
#include "game_objects_custom_uniforms.h"
#include "ces_material_component.h"
#include "texture.h"
#include "texture_loading_operation.h"
#include "db_garage_table.h"
#include "db_car_table.h"
#include "db_user_table.h"
#include "ces_garage_database_component.h"
#include "db_level_table.h"
#include "ces_levels_database_component.h"

namespace game
{
    i32 gameplay_fabricator::g_character_guid = 0;
    
    std::vector<std::string> g_nicknames = {
        "Hightower",
        "Smurf",
        "Pixel",
        "Butcher",
        "Pepper",
        "Houston",
        "Wizard",
        "Genius",
        "Hyper",
        "Pluto",
        "Alpha",
        "Jester",
        "Pogue",
        "Hobo",
        "Jigsaw",
        "Prometheus",
        "Angler",
        "Joker",
        "Psycho",
        "King",
        "Judge",
        "Pusher",
        "Blister",
        "K-9",
        "Roadblock",
        "Bowie",
        "Rooster",
        "Bowler",
        "Breadmaker",
        "Scrapper",
        "Broomspun",
        "Kingfisher",
        "Screwtape",
        "Chocolate",
        "Bugger",
        "Knuckles",
        "Chaser",
        "Cabbie",
        "Killer",
        "Gladiator",
        "Cobra",
        "Enforcer",
        "Peroxide",
        "Charger",
        "Zod" };

    
    glm::vec3 gameplay_fabricator::k_car_01_wheel_fl_offset = glm::vec3(.9f, .32f, 1.1f);
    glm::vec3 gameplay_fabricator::k_car_01_wheel_fr_offset = glm::vec3(-.9f, .32f, 1.1f);
    glm::vec3 gameplay_fabricator::k_car_01_wheel_rl_offset = glm::vec3(.9f, .32f, -1.2f);
    glm::vec3 gameplay_fabricator::k_car_01_wheel_rr_offset = glm::vec3(-.9f, .32f, -1.2f);
    
    glm::vec3 gameplay_fabricator::k_car_01_light_rl_offset = glm::vec3(.55f, .75f, -2.3f);;
    glm::vec3 gameplay_fabricator::k_car_01_light_rr_offset = glm::vec3(-.55f, .75f, -2.3f);
    
    gameplay_fabricator::gameplay_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator) :
    m_general_fabricator(general_fabricator)
    {
        m_gameplay_configuration_accessor = std::make_shared<gb::gameplay_configuration_accessor>();
        
        m_database_coordinator = std::make_shared<gb::db::database_coordinator>();
        m_database_coordinator->open("game.db");
        m_database_coordinator->register_table<db_user_table>();
        m_database_coordinator->register_table<db_garage_table>();
        m_database_coordinator->register_table<db_car_table>();
        m_database_coordinator->register_table<db_level_table>();
    }
    
    gb::db::database_coordinator_shared_ptr gameplay_fabricator::get_database_coordinator() const
    {
        return m_database_coordinator;
    }
    
    std::shared_ptr<gb::levels_set_configuration> gameplay_fabricator::get_levels_set_configuration(const std::string& filename) const
    {
          const auto levels_set_configuration = std::static_pointer_cast<gb::levels_set_configuration>(m_gameplay_configuration_accessor->get_levels_set_configuration(filename));
        return levels_set_configuration;
    }
    
    std::shared_ptr<gb::cars_progression_configuration> gameplay_fabricator::get_cars_progression_configuration(const std::string& filename) const
    {
        const auto cars_progression_configuration = std::static_pointer_cast<gb::cars_progression_configuration>(m_gameplay_configuration_accessor->get_cars_progression_configuration(filename));
        return cars_progression_configuration;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_scene(const std::string& filename)
    {
        auto scene = gb::ces_entity::construct<gb::game_object_3d>();
        
        const auto general_fabricator = m_general_fabricator.lock();
        const auto resource_accessor = general_fabricator->get_resource_accessor();
        
        const auto scene_2d = resource_accessor->get_resource<gb::scene_2d, gb::scene_2d_loading_operation>(filename, true);
        
        const auto level_route_component = std::make_shared<ces_level_route_component>();
        scene->add_component(level_route_component);
        
        const auto level_descriptor_component = std::make_shared<ces_level_descriptor_component>();
        scene->add_component(level_descriptor_component);
        
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
                    
                    const auto shader_uniforms_component = std::make_shared<gb::ces_shader_uniforms_component>();
                    shader_uniforms_component->construct_uniforms<sky_reflection_shader_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_type_vertex);
                    road_straight->add_component(shader_uniforms_component);
                    
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
        for (const auto& spawner : spawners)
        {
            auto position = spawner->get_position();
            position.x /= scene_2d->get_tile_size().x;
            position.y /= scene_2d->get_tile_size().y;
            position.x *= 16.f;
            position.y *= -16.f;
            
            level_route_component->add_spawner_point(position);
        }
        
        const auto walls = scene_2d->get_objects("walls");
        for (const auto& wall : walls)
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
                box2d_body_component->set_deferred_box2d_component_setup(wall_object_3d, b2BodyType::b2_staticBody, [hx, hy, angle](gb::ces_entity_const_shared_ptr entity, gb::ces_box2d_body_component_const_shared_ptr component) {
                    gb::ces_box2d_body_component::box2d_shape_parameters parameters;
                    parameters.m_shape = gb::ces_box2d_body_component::box;
                    parameters.m_hx = hx;
                    parameters.m_hy = hy;
                    parameters.m_center = glm::vec2(0.f);
                    parameters.m_angle = angle;
                    component->add_shape_parameters(parameters);
                });
                box2d_body_component->set_custom_box2d_body_setup([](gb::ces_entity_const_shared_ptr entity,
                                                                     gb::ces_box2d_body_component_const_shared_ptr component,
                                                                     b2Body* box2d_body,
                                                                     std::vector<std::shared_ptr<b2Shape>> box2d_shapes) {
                    
                    std::shared_ptr<b2Shape> shape = box2d_shapes.at(0);
                    const auto fixture = box2d_body->CreateFixture(shape.get(), 1.f);
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
                    //fixture->SetFilterData(filter);
                });
                wall_object_3d->add_component(box2d_body_component);
                scene->add_child(wall_object_3d);
                from = to;
            }
        }
        
        const auto lights = scene_2d->get_objects("lights");
        for (const auto& light : lights)
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
        
        const auto slow_motion_triggers = scene_2d->get_objects("slow_motion_triggers");
        for (const auto& slow_motion_trigger : slow_motion_triggers)
        {
            auto position = slow_motion_trigger->get_position();
            position.x /= scene_2d->get_tile_size().x;
            position.y /= scene_2d->get_tile_size().y;
            position.x *= 16.f;
            position.y *= -16.f;
            
            level_route_component->add_slow_motion_trigger(position);
        }
        
        const auto trees = scene_2d->get_objects("trees");
        for (const auto& tree : trees)
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
        for (const auto& building : buildings)
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
            building_object->position = glm::vec3(position.x, -.1f, position.y);
            building_object->rotation = glm::vec3(0.f, building->get_rotation(), 0.f);
        }
        
        const auto routes = scene_2d->get_objects("route");
        for (const auto& route : routes)
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
                level_route_component->add_route_point(point_position);
            }
        }
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_3d_component>();
        bound_touch_component->set_min_bound(glm::vec3(-4096.f, 0.f, -4096.f));
        bound_touch_component->set_max_bound(glm::vec3(4096.f, 0.f, 4096.f));
        scene->add_component(bound_touch_component);
        
        return scene;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_car(const std::string& filename)
    {
        std::string configuration_filename = filename;
        configuration_filename.append("_configuration.xml");
        const auto car_configuration = std::static_pointer_cast<gb::car_configuration>(m_gameplay_configuration_accessor->get_car_configuration(configuration_filename));
        
        const auto car_body = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_main_3d_configuration_filename());
        car_body->tag = ces_car_parts_component::parts::k_body;
        
        std::stringstream car_guid;
        car_guid<<filename<<g_character_guid++;
        const auto car = gb::ces_entity::construct<gb::game_object_3d>();
        car->tag = car_guid.str();
        car->add_child(car_body);
        
        f32 f_wheels_scale = car_configuration->get_f_wheels_scale();
        f32 r_wheels_scale = car_configuration->get_r_wheels_scale();
        
        const auto car_fl_wheel_container = gb::ces_entity::construct<gb::game_object_3d>();
        const auto car_fl_wheel = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_wheel_l_3d_configuration_filename());
        car_fl_wheel_container->position = glm::vec3(car_configuration->get_fl_wheel_offset_x(),
                                                     car_configuration->get_fl_wheel_offset_y(),
                                                     car_configuration->get_fl_wheel_offset_z());
        car_fl_wheel->scale = glm::vec3(f_wheels_scale);
        car_fl_wheel_container->add_child(car_fl_wheel);
        car->add_child(car_fl_wheel_container);
        
        const auto car_tire_rl = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_wheel_l_3d_configuration_filename());
        car_tire_rl->position = glm::vec3(car_configuration->get_rl_wheel_offset_x(),
                                          car_configuration->get_rl_wheel_offset_y(),
                                          car_configuration->get_rl_wheel_offset_z());
        
        car_tire_rl->scale = glm::vec3(r_wheels_scale);
        car->add_child(car_tire_rl);
        
        const auto car_fr_wheel_container = gb::ces_entity::construct<gb::game_object_3d>();
        const auto car_fr_wheel = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_wheel_r_3d_configuration_filename());
        car_fr_wheel_container->position = glm::vec3(car_configuration->get_fr_wheel_offset_x(),
                                                     car_configuration->get_fr_wheel_offset_y(),
                                                     car_configuration->get_fr_wheel_offset_z());
        car_fr_wheel->scale = glm::vec3(f_wheels_scale);
        car_fr_wheel_container->add_child(car_fr_wheel);
        car->add_child(car_fr_wheel_container);
        
        const auto car_tire_rr = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_wheel_r_3d_configuration_filename());
        car_tire_rr->position = glm::vec3(car_configuration->get_rr_wheel_offset_x(),
                                          car_configuration->get_rr_wheel_offset_y(),
                                          car_configuration->get_rr_wheel_offset_z());
        car_tire_rr->scale = glm::vec3(r_wheels_scale);
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
        particle_emitter_smoke_01->position = glm::vec3(-.7f, .1f, -1.55f);
        car->add_child(particle_emitter_smoke_01);
        
        const auto particle_emitter_smoke_02 = m_general_fabricator.lock()->create_particle_emitter("particle.emitter.smoke.xml");
        particle_emitter_smoke_02->position = glm::vec3(.7f, .1f, -1.55f);
        car->add_child(particle_emitter_smoke_02);
        
        const auto light_source_01 = m_general_fabricator.lock()->create_deferred_spot_light_3d("cone_light_source.xml");
        car->add_child(light_source_01);
        light_source_01->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        light_source_01->position = glm::vec3(.25f, .75f, .0f);
        light_source_01->rotation = glm::vec3(0.f, 10.f, 0.f);
        light_source_01->outer_cutoff_angle = glm::cos(glm::radians(15.f));
        light_source_01->inner_cutoff_angle = glm::cos(glm::radians(10.f));
        light_source_01->scale = glm::vec3(5.f);

        const auto light_source_02 = m_general_fabricator.lock()->create_deferred_spot_light_3d("cone_light_source.xml");
        car->add_child(light_source_02);
        light_source_02->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        light_source_02->position = glm::vec3(-.25f, .75f, .0f);
        light_source_02->rotation = glm::vec3(0.f, -10.f, 0.f);
        light_source_02->outer_cutoff_angle = glm::cos(glm::radians(15.f));
        light_source_02->inner_cutoff_angle = glm::cos(glm::radians(10.f));
        light_source_02->scale = glm::vec3(5.f);
        
        const auto light_rr = m_general_fabricator.lock()->create_deferred_point_light_3d("omni_light_source.xml");
        car_body->add_child(light_rr);
        light_rr->ray_length = 1.2f;
        light_rr->color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        light_rr->position = glm::vec3(car_configuration->get_rr_light_offset_x(),
                                       car_configuration->get_rr_light_offset_y(),
                                       car_configuration->get_rr_light_offset_z());
        
        const auto light_rl = m_general_fabricator.lock()->create_deferred_point_light_3d("omni_light_source.xml");
        car_body->add_child(light_rl);
        light_rl->ray_length = 1.2f;
        light_rl->color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        light_rl->position = glm::vec3(car_configuration->get_rl_light_offset_x(),
                                       car_configuration->get_rl_light_offset_y(),
                                       car_configuration->get_rl_light_offset_z());
        
        const auto light_fl = m_general_fabricator.lock()->create_deferred_point_light_3d("omni_light_source.xml");
        car_body->add_child(light_fl);
        light_fl->ray_length = .6f;
        light_fl->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        light_fl->position = glm::vec3(car_configuration->get_fl_light_offset_x(),
                                       car_configuration->get_fl_light_offset_y(),
                                       car_configuration->get_fl_light_offset_z());
        
        const auto light_fr = m_general_fabricator.lock()->create_deferred_point_light_3d("omni_light_source.xml");
        car_body->add_child(light_fr);
        light_fr->ray_length = .6f;
        light_fr->color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        light_fr->position = glm::vec3(car_configuration->get_fr_light_offset_x(),
                                       car_configuration->get_fr_light_offset_y(),
                                       car_configuration->get_fr_light_offset_z());
        
        const auto car_parts_component = std::make_shared<ces_car_parts_component>();
        car_parts_component->add_part(car_body, ces_car_parts_component::parts::k_body);
        car_parts_component->add_part(car_fl_wheel_container, ces_car_parts_component::parts::k_fl_wheel_container);
        car_parts_component->add_part(car_fr_wheel_container, ces_car_parts_component::parts::k_fr_wheel_container);
        car_parts_component->add_part(car_fl_wheel, ces_car_parts_component::parts::k_fl_tire);
        car_parts_component->add_part(car_fr_wheel, ces_car_parts_component::parts::k_fr_tire);
        car_parts_component->add_part(car_tire_rl, ces_car_parts_component::parts::k_rl_tire);
        car_parts_component->add_part(car_tire_rr, ces_car_parts_component::parts::k_rr_tire);
        car_parts_component->add_part(light_fl, ces_car_parts_component::parts::k_fl_light);
        car_parts_component->add_part(light_fr, ces_car_parts_component::parts::k_fr_light);
        car_parts_component->add_part(light_rl, ces_car_parts_component::parts::k_bl_light);
        car_parts_component->add_part(light_rr, ces_car_parts_component::parts::k_br_light);
        car_parts_component->add_part(light_source_01, ces_car_parts_component::parts::k_light_fl_direction);
        car_parts_component->add_part(light_source_02, ces_car_parts_component::parts::k_light_fr_direction);
        car_parts_component->add_part(particle_emitter_smoke_01, ces_car_parts_component::parts::k_rl_tire_particles);
        car_parts_component->add_part(particle_emitter_smoke_02, ces_car_parts_component::parts::k_rr_tire_particles);
        car->add_component(car_parts_component);
        
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
        box2d_body_component->set_deferred_box2d_component_setup(car, b2BodyType::b2_dynamicBody, [car_configuration](gb::ces_entity_const_shared_ptr entity,         gb::ces_box2d_body_component_const_shared_ptr component) {
            
            gb::ces_box2d_body_component::box2d_shape_parameters shape_parameters;
            shape_parameters.m_shape = gb::ces_box2d_body_component::circle;
            shape_parameters.set_radius(1.4f);
            shape_parameters.m_center = glm::vec2(0.f, 0.f);
            component->add_shape_parameters(shape_parameters);
            
            /*std::vector<b2Vec2> vertices;
            for (i32 i = 0; i < 36; i++)
            {
                f32 angle = ( i * 2 * M_PI) / 36;
                f32 x, y;
                x = 1.2f * cosf(angle);
                y = 2.4f * sinf(angle);
                vertices.push_back(b2Vec2(x, y));
            }
            
            for (i32 i = 0; i < vertices.size(); ++i)
            {
                const auto point_a = vertices.at(i);
                const auto point_b = vertices.at((i + 1) % vertices.size());
                std::vector<b2Vec2> points;
                points.push_back(point_a);
                points.push_back(point_b);
                gb::ces_box2d_body_component::box2d_shape_parameters shape_parameters;
                shape_parameters.m_shape = gb::ces_box2d_body_component::edge;
                shape_parameters.set_custom_vertices(points);
                component->add_shape_parameters(shape_parameters);
            }*/
        });
        box2d_body_component->set_custom_box2d_body_setup([](gb::ces_entity_const_shared_ptr entity, gb::ces_box2d_body_component_const_shared_ptr component, b2Body* box2d_body, std::vector<std::shared_ptr<b2Shape>> box2d_shapes) {
            const auto car_model_component = entity->get_component<ces_car_model_component>();
            const auto car_input_component = entity->get_component<ces_car_input_component>();
            
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
            
            for (const auto &shape_it : box2d_shapes)
            {
                const auto fixture = box2d_body->CreateFixture(shape_it.get(), car_model_component->get_density());
                fixture->SetFriction(car_model_component->get_friction());
                fixture->SetRestitution(car_model_component->get_restitution());
                //fixture->SetFilterData(filter);
            }
            
            b2MassData* box2d_mass_data = new b2MassData();
            box2d_body->GetMassData(box2d_mass_data);
            box2d_mass_data->center.Set(0, 0);
            box2d_body->SetMassData(box2d_mass_data);
            box2d_body->SetBullet(true);
        });
        car->add_component(box2d_body_component);
        
        const auto shader_uniforms_component = std::make_shared<gb::ces_shader_uniforms_component>();
        shader_uniforms_component->construct_uniforms<sky_reflection_shader_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_type_vertex);
        car_body->add_component(shader_uniforms_component);
        return car;
    }
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_player_car(const std::string& filename)
    {
        const auto car = create_car(filename);
        
        auto car_statistic_component = std::make_shared<ces_car_statistic_component>();
        car_statistic_component->mode = ces_car_statistic_component::e_mode_player;
        car->add_component(car_statistic_component);
        
        auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
        car_descriptor_component->racer_name = "player";
        
        const auto name_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        name_label->text = "racer";
        name_label->font_size = 24;
        name_label->font_mode = gb::ces_font_component::e_font_mode_edge;
        name_label->font_color = glm::u8vec4(255, 255, 255, 255);
        name_label->scale = glm::vec3(.05f);
        name_label->position = glm::vec3(name_label->get_content_size().x * .5f, 1.f, -3.f);
        name_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(name_label);
        
        const auto speed_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        speed_label->text = "speed";
        speed_label->font_size = 24;
        speed_label->font_mode = gb::ces_font_component::e_font_mode_edge;
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
        drift_label->text = "DRIFT";
        drift_label->font_size = 24;
        drift_label->font_mode = gb::ces_font_component::e_font_mode_edge;
        drift_label->font_color = glm::u8vec4(255, 255, 255, 255);
        drift_label->position = glm::vec3(speed_label->get_content_size().x - 5.7f, 1.f, 0.f);
        drift_label->scale = glm::vec3(.05f);
        drift_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(drift_label);
        
        const auto drift_value_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        drift_value_label->text = "00:00 sec";
        drift_value_label->font_size = 24;
        drift_value_label->font_color = glm::u8vec4(255, 255, 255, 255);
        drift_value_label->position = glm::vec3(speed_label->get_content_size().x - 5.7f, 1.f, -1.25f);
        drift_value_label->scale = glm::vec3(.025f);
        drift_value_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(drift_value_label);
        
        const auto rpm_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        rpm_label->text = "rpm";
        rpm_label->font_size = 24;
        rpm_label->font_mode = gb::ces_font_component::e_font_mode_edge;
        rpm_label->font_color = glm::u8vec4(255, 255, 255, 255);
        rpm_label->position = glm::vec3(speed_label->get_content_size().x + 1.7f, 1.f, 0.f);
        rpm_label->scale = glm::vec3(.05f);
        rpm_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(rpm_label);
        
        const auto rpm_value_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        rpm_value_label->text = "0";
        rpm_value_label->font_size = 24;
        rpm_value_label->font_color = glm::u8vec4(255, 255, 255, 255);
        rpm_value_label->position = glm::vec3(speed_label->get_content_size().x + 1.7f, 1.f, -1.25f);
        rpm_value_label->scale = glm::vec3(.025f);
        rpm_value_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(rpm_value_label);
        
        const auto countdown_value_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        countdown_value_label->text = "3";
        countdown_value_label->font_size = 48;
        countdown_value_label->scale = glm::vec3(.33f);
        countdown_value_label->font_color = glm::u8vec4(255, 255, 255, 255);
        countdown_value_label->position = glm::vec3(countdown_value_label->get_content_size().x * .5f, 3.f, 4.f);
        countdown_value_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(countdown_value_label);
        
        const auto car_direction_arrow = m_general_fabricator.lock()->create_shape_3d("arrow.xml", "plane");
        car_direction_arrow->position = glm::vec3(0.f, .5f, 6.f);
        car_direction_arrow->scale = glm::vec3(3.f, 3.f, 3.f);
        car_direction_arrow->rotation = glm::vec3(90.f, 0.f, 0.f);
        car->add_child(car_direction_arrow);
        
        const auto car_parts_component = car->get_component<ces_car_parts_component>();
        car_parts_component->add_part(name_label, ces_car_parts_component::parts::k_ui_name_label);
        car_parts_component->add_part(speed_label, ces_car_parts_component::parts::k_ui_speed_label);
        car_parts_component->add_part(speed_value_label, ces_car_parts_component::parts::k_ui_speed_value_label);
        car_parts_component->add_part(drift_label, ces_car_parts_component::parts::k_ui_drift_label);
        car_parts_component->add_part(drift_value_label, ces_car_parts_component::parts::k_ui_drift_value_label);
        car_parts_component->add_part(rpm_label, ces_car_parts_component::parts::k_ui_rpm_label);
        car_parts_component->add_part(rpm_value_label, ces_car_parts_component::parts::k_ui_rpm_value_label);
        car_parts_component->add_part(countdown_value_label, ces_car_parts_component::parts::k_ui_countdown_value_label);
        car_parts_component->add_part(car_direction_arrow, ces_car_parts_component::parts::k_ui_direction_arrow);
        
        const auto car_replay_record_component = std::make_shared<ces_car_replay_record_component>();
        std::stringstream replay_record_guid;
        replay_record_guid<<"racer_"<<std::get_tick_count();
        car_replay_record_component->set_record_guid(replay_record_guid.str());
        car->add_component(car_replay_record_component);
        
        const auto car_gear_component = std::make_shared<ces_car_gear_component>();
        car->add_component(car_gear_component);
        
        const auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_drift, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_drift, false);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_drift, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_idle, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_idle, false);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_idle, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_off_low, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_off_low, false);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_low, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_off_mid, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_off_mid, false);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_mid, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_off_high, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_off_high, false);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_high, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_on_low, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_on_low, false);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_low, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_on_mid, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_on_mid, false);
        sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_mid, 0.f);
        
        sound_component->add_sound(ces_car_sounds_set_component::sounds::k_engine_on_high, true);
        sound_component->trigger_sound(ces_car_sounds_set_component::sounds::k_engine_on_high, false);
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
    
    gb::game_object_3d_shared_ptr gameplay_fabricator::create_ai_car(const std::string& filename)
    {
        const auto car = create_car(filename);
        car->remove_component(ces_car_input_component::class_guid());
        car->add_component(std::make_shared<ces_car_ai_input_component>());
        
        auto car_statistic_component = std::make_shared<ces_car_statistic_component>();
        car_statistic_component->mode = ces_car_statistic_component::e_mode_ai;
        car->add_component(car_statistic_component);
        
        car->get_component<ces_car_model_component>()->set_max_force(std::get_random_f(220.f, 320.f));
        
        auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
        std::string racer_name = g_nicknames.at(std::get_random_i(0, static_cast<i32>(g_nicknames.size()) - 1));
        std::transform(racer_name.begin(), racer_name.end(), racer_name.begin(), ::toupper);
        car_descriptor_component->racer_name = racer_name;
        
        const auto name_label = m_general_fabricator.lock()->create_label_3d("information_bubble_01.xml");
        name_label->text = racer_name;
        name_label->font_size = 24;
        name_label->font_color = glm::u8vec4(255, 255, 255, 255);
        name_label->scale = glm::vec3(.05f);
        name_label->position = glm::vec3(name_label->get_content_size().x * .5f, 1.f, -3.f);
        name_label->rotation = glm::vec3(-90.f, 180.f, 0.f);
        car->add_child(name_label);
        const auto car_parts_component = car->get_component<ces_car_parts_component>();
        car_parts_component->add_part(name_label, ces_car_parts_component::parts::k_ui_name_label);
        
        // const auto car_replay_player_component = std::make_shared<ces_car_replay_player_component>();
        // car->add_component(car_replay_player_component);
        
        /*std::string replay_filename = bundlepath();
        replay_filename.append("racer_79950480");
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
            //car_replay_player_component->set_record(record);
        }*/
        
        return car;
    }
    
    void gameplay_fabricator::place_car_on_level(const gb::game_object_3d_shared_ptr& level, const gb::game_object_3d_shared_ptr &car, i32 spawner_position)
    {
        const auto level_route_component = level->get_component<ces_level_route_component>();
        std::vector<glm::vec2> route = level_route_component->route;
        std::vector<glm::vec2> spawners = level_route_component->spawners;
        i32 nearest_next_checkpoint_index = 0;
        f32 nearest_next_checkpoint_distance = glm::distance(glm::vec2(spawners.at(spawner_position).x,
                                                                       spawners.at(spawner_position).y),
                                                             route.at(nearest_next_checkpoint_index));
        
        i32 index = 0;
        for (auto route_it : route)
        {
            f32 distance = glm::distance(glm::vec2(spawners.at(spawner_position).x,
                                                   spawners.at(spawner_position).y), route_it);
            if (distance < nearest_next_checkpoint_distance)
            {
                nearest_next_checkpoint_distance = distance;
                nearest_next_checkpoint_index = index;
            }
            index++;
        }
        
        i32 near_checkpoint_index = (nearest_next_checkpoint_index + 1) % route.size();
        const auto near_checkpoint_position = route.at(near_checkpoint_index);
        f32 rotation = atan2(near_checkpoint_position.x - spawners.at(spawner_position).x,
                                  near_checkpoint_position.y - spawners.at(spawner_position).y);
        rotation = glm::wrap_radians(rotation);
        
        if (rotation < 0.f)
        {
            rotation += M_PI * 2.f;
        }
        
        if (rotation > M_PI)
        {
            rotation -= M_PI * 2.f;
            
        }
        
        car->position = glm::vec3(spawners.at(spawner_position).x, 0.f, spawners.at(spawner_position).y);
        car->rotation = glm::vec3(0.f, glm::degrees(rotation), 0.f);
    }
    
    void gameplay_fabricator::reconstruct_car_geometry(const gb::game_object_3d_shared_ptr& car, const std::string& filename)
    {
        std::string configuration_filename = filename;
        configuration_filename.append("_configuration.xml");
        const auto car_configuration = std::static_pointer_cast<gb::car_configuration>(m_gameplay_configuration_accessor->get_car_configuration(configuration_filename));
        
        const auto car_parts_component = car->get_component<ces_car_parts_component>();
        auto car_body = car_parts_component->get_part(ces_car_parts_component::parts::k_body);
        
        const auto light_fl = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_fl_light));
        light_fl->remove_from_parent();
        const auto light_fr = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_fr_light));
        light_fr->remove_from_parent();
        const auto light_rl = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_bl_light));
        light_rl->remove_from_parent();
        const auto light_rr = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_br_light));
        light_rr->remove_from_parent();
        
        car_body->remove_from_parent();
        car_body = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_main_3d_configuration_filename());
        car_body->tag = ces_car_parts_component::parts::k_body;
        car_body->add_child(light_fl);
        car_body->add_child(light_fr);
        car_body->add_child(light_rl);
        car_body->add_child(light_rr);
        car->add_child(car_body);
        
        light_fl->position = glm::vec3(car_configuration->get_fl_light_offset_x(),
                                       car_configuration->get_fl_light_offset_y(),
                                       car_configuration->get_fl_light_offset_z());
        
        light_fr->position = glm::vec3(car_configuration->get_fr_light_offset_x(),
                                       car_configuration->get_fr_light_offset_y(),
                                       car_configuration->get_fr_light_offset_z());
        
        light_rl->position = glm::vec3(car_configuration->get_rl_light_offset_x(),
                                       car_configuration->get_rl_light_offset_y(),
                                       car_configuration->get_rl_light_offset_z());
        
        light_rr->position = glm::vec3(car_configuration->get_rr_light_offset_x(),
                                       car_configuration->get_rr_light_offset_y(),
                                       car_configuration->get_rr_light_offset_z());
        
        const auto shader_uniforms_component = std::make_shared<gb::ces_shader_uniforms_component>();
        shader_uniforms_component->construct_uniforms<sky_reflection_shader_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_type_vertex);
        car_body->add_component(shader_uniforms_component);
        
        auto car_fl_wheel = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_fl_tire));
        car_fl_wheel->remove_from_parent();
        
        auto car_fr_wheel = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_fr_tire));
        car_fr_wheel->remove_from_parent();
        
        auto car_rl_wheel = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_rl_tire));
        car_rl_wheel->remove_from_parent();
        
        auto car_rr_wheel = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_rr_tire));
        car_rr_wheel->remove_from_parent();
        
        f32 f_wheels_scale = car_configuration->get_f_wheels_scale();
        f32 r_wheels_scale = car_configuration->get_r_wheels_scale();
        
        const auto car_fl_wheel_container = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_fl_wheel_container));
        car_fl_wheel = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_wheel_l_3d_configuration_filename());
        car_fl_wheel_container->position = glm::vec3(car_configuration->get_fl_wheel_offset_x(),
                                                     car_configuration->get_fl_wheel_offset_y(),
                                                     car_configuration->get_fl_wheel_offset_z());
        car_fl_wheel->scale = glm::vec3(f_wheels_scale);
        car_fl_wheel_container->add_child(car_fl_wheel);
        
        car_rl_wheel = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_wheel_l_3d_configuration_filename());
        car_rl_wheel->position = glm::vec3(car_configuration->get_rl_wheel_offset_x(),
                                           car_configuration->get_rl_wheel_offset_y(),
                                           car_configuration->get_rl_wheel_offset_z());
        car_rl_wheel->scale = glm::vec3(r_wheels_scale);
        car->add_child(car_rl_wheel);
        
        const auto car_fr_wheel_container = std::static_pointer_cast<gb::game_object_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_fr_wheel_container));
        car_fr_wheel = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_wheel_r_3d_configuration_filename());
        car_fr_wheel_container->position = glm::vec3(car_configuration->get_fr_wheel_offset_x(),
                                                     car_configuration->get_fr_wheel_offset_y(),
                                                     car_configuration->get_fr_wheel_offset_z());
        car_fr_wheel->scale = glm::vec3(f_wheels_scale);
        car_fr_wheel_container->add_child(car_fr_wheel);
        
        car_rr_wheel = m_general_fabricator.lock()->create_shape_3d(car_configuration->get_wheel_r_3d_configuration_filename());
        car_rr_wheel->position = glm::vec3(car_configuration->get_rr_wheel_offset_x(),
                                           car_configuration->get_rr_wheel_offset_y(),
                                           car_configuration->get_rr_wheel_offset_z());
        car_rr_wheel->scale = glm::vec3(r_wheels_scale);
        car->add_child(car_rr_wheel);
        
        auto car_tire_trails_controller_component = std::make_shared<ces_car_tire_trails_controller_component>();
        car->get_component<ces_car_tire_trails_controller_component>()->set_parameters("tire_trail.xml", m_general_fabricator.lock(), car_rl_wheel, car_rr_wheel);
        
        car_parts_component->add_part(car_body, ces_car_parts_component::parts::k_body);
        car_parts_component->add_part(car_fl_wheel, ces_car_parts_component::parts::k_fl_tire);
        car_parts_component->add_part(car_fr_wheel, ces_car_parts_component::parts::k_fr_tire);
        car_parts_component->add_part(car_rl_wheel, ces_car_parts_component::parts::k_rl_tire);
        car_parts_component->add_part(car_rr_wheel, ces_car_parts_component::parts::k_rr_tire);
    }
    
    void gameplay_fabricator::reskin_car(const gb::game_object_3d_shared_ptr& car, const std::string& filename, ui32 skin_index)
    {
        std::string configuration_filename = filename;
        configuration_filename.append("_configuration.xml");
        const auto car_configuration = std::static_pointer_cast<gb::car_configuration>(m_gameplay_configuration_accessor->get_car_configuration(configuration_filename));
        
        const auto car_parts_component = car->get_component<ces_car_parts_component>();
        const auto car_body = car_parts_component->get_part(ces_car_parts_component::parts::k_body);
        const auto material_compomnent = car_body->get_component<gb::ces_material_component>();
        
        std::string skin_filename;
        if (skin_index == 1)
        {
            skin_filename = car_configuration->get_skin_1_filename();
        }
        
        if (skin_index == 2)
        {
            skin_filename = car_configuration->get_skin_2_filename();
        }
        
        if (skin_index == 3)
        {
            skin_filename = car_configuration->get_skin_3_filename();
        }
        
        const auto general_fabricator = m_general_fabricator.lock();
        const auto resource_accessor = general_fabricator->get_resource_accessor();
        auto texture = resource_accessor->get_resource<gb::texture, gb::texture_loading_operation>(skin_filename, true);
        material_compomnent->set_texture(texture, gb::e_shader_sampler::e_shader_sampler_01);
    }
}
