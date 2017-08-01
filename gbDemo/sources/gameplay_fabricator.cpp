//
//  gameplay_fabricator.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "gameplay_fabricator.h"
#include "gameplay_configuration_accessor.h"
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
#include "ces_bound_touch_component.h"
#include "ces_ui_interaction_component.h"
#include "ces_ui_avatar_icon_component.h"
#include "vbo.h"
#include "path_map.h"
#include "shape_3d.h"
#include "light_source_2d.h"
#include "ces_transformation_extension.h"
#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"
#include "hit_bounds_controller.h"
#include "db_character_entity.h"
#include "db_character_entity_controller.h"

namespace game
{
    gameplay_fabricator::gameplay_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                                             const gb::anim::anim_fabricator_shared_ptr& anim_fabricator) :
    m_general_fabricator(general_fabricator),
    m_anim_fabricator(anim_fabricator)
    {
        m_gameplay_configuration_accessor = std::make_shared<gb::gameplay_configuration_accessor>();
        m_characters_3d_assembler = std::make_shared<characters_3d_assembler>();
    }
    
    gb::game_object_2d_shared_ptr gameplay_fabricator::create_level(const std::string& filename)
    {
        auto database_entities_controllers = std::make_shared<gb::db::database_entities_controllers>();
        database_entities_controllers->open("game.db");
        database_entities_controllers->register_entity_controller<db_character_entity_controller>();
        
        auto db_character_entity = std::make_shared<game::db_character_entity>(database_entities_controllers);
        db_character_entity->load_from_db(1);
        
        auto level_configuration = std::static_pointer_cast<gb::level_configuration>(m_gameplay_configuration_accessor->get_level_configuration(filename));
        auto level = gb::ces_entity::construct<gb::game_object_2d>();
        
        auto level_layers_component = std::make_shared<ces_level_layers_component>();
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
        return level;
    }
    
    gb::game_object_2d_shared_ptr gameplay_fabricator::create_character(const std::string& filename, const std::array<gb::ces_entity_weak_ptr, ces_level_layers_component::e_level_layer_max>& layers)
    {
        auto character_configuration = std::static_pointer_cast<gb::character_configuration>(m_gameplay_configuration_accessor->get_character_configuration(filename));
        auto character_main_3d_entity = m_general_fabricator.lock()->create_shape_3d(character_configuration->get_main_3d_configuration_filename());
        auto character_main_2d_entity = m_general_fabricator.lock()->create_sprite(character_configuration->get_main_2d_configuration_filename());
        auto character_avatar_3d_entity = m_general_fabricator.lock()->create_shape_3d(character_configuration->get_avatar_3d_configuration_filename());
        auto character_avatar_2d_entity = m_general_fabricator.lock()->create_sprite(character_configuration->get_avatar_2d_configuration_filename());
        
        m_characters_3d_assembler->assemble(character_main_2d_entity, character_main_3d_entity, glm::vec2(character_configuration->get_visual_size()), characters_3d_assembler::e_view_type_top);
        m_characters_3d_assembler->assemble(character_avatar_2d_entity, character_avatar_3d_entity, glm::vec2(character_configuration->get_visual_size()), characters_3d_assembler::e_view_type_front);
        character_avatar_2d_entity->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        character_avatar_2d_entity->position = glm::vec2(40.f, 70.f);
        character_avatar_3d_entity->play_animation(ces_character_animation_component::animations::k_search_animation, true);
        
        gb::game_object_2d_shared_ptr character = gb::ces_entity::construct<gb::game_object_2d>();
        character->tag = filename;
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
        character_controllers_component->mode = ces_character_controllers_component::e_mode_manual;
        
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
        character_statistic_component->setup(character_configuration->get_hp(),
                                             character_configuration->get_move_speed(),
                                             character_configuration->get_attack_speed(),
                                             character_configuration->get_damage(),
                                             character_configuration->get_attack_distance(),
                                             character_configuration->get_reviving_time());
        character_statistic_component->set_spawn_position(glm::vec2(character_configuration->get_spawn_position_x(),
                                                                    character_configuration->get_spawn_position_y()));
        auto health_status_entity = m_general_fabricator.lock()->create_sprite("character.statistic.2d.xml");
        health_status_entity->tag = "character.statistic.2d.xml";
        health_status_entity->size = glm::vec2(96.f);
        health_status_entity->pivot = glm::vec2(.5f, .5f);
        health_status_entity->color = glm::u8vec4(0, 255, 0, 255);
        layers[ces_level_layers_component::e_level_layer_characters_down_statistic].lock()->add_child(health_status_entity);
        character_statistic_component->setup(health_status_entity);
        
        character->add_component(character_statistic_component);
        
        auto character_selector_component = std::make_shared<ces_character_selector_component>();
        character->add_component(character_selector_component);
        
        auto character_animation_component = std::make_shared<ces_character_animation_component>();
        character_animation_component->set_3d_entity_linkage(character_main_3d_entity);
        character->add_component(character_animation_component);
        
        auto character_state_automat_component = std::make_shared<ces_character_state_automat_component>();
        character_state_automat_component->set_mode(ces_character_state_automat_component::e_mode_manual);
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
        
        return character;
    }
    
    gb::game_object_2d_shared_ptr gameplay_fabricator::create_mob(const std::string& filename, const std::array<gb::ces_entity_weak_ptr, ces_level_layers_component::e_level_layer_max>& layers)
    {
        auto mob_configuration = std::static_pointer_cast<gb::mob_configuration>(m_gameplay_configuration_accessor->get_mob_configuration(filename));
        auto character = gameplay_fabricator::create_character(mob_configuration->get_character_configuration_filename(), layers);
        auto character_controllers_component = character->get_component<ces_character_controllers_component>();
        character_controllers_component->mode = ces_character_controllers_component::e_mode_ai;
        auto character_state_automat_component = character->get_component<ces_character_state_automat_component>();
        character_state_automat_component->set_mode(game::ces_character_state_automat_component::e_mode_ai);
        auto character_statistic_component = character->get_component<ces_character_statistic_component>();
        character_statistic_component->setup(mob_configuration->get_chase_start_distance(),
                                             mob_configuration->get_chase_end_distance());
        auto character_parts_component = character->get_component<ces_character_parts_component>();
        auto light_source = std::static_pointer_cast<gb::light_source_2d>(character_parts_component->get_light_source_part());
        light_source->color = glm::vec4(1.f, 0.f, 0.f, 1.f);
        return character;
    }
    
    gb::game_object_2d_shared_ptr gameplay_fabricator::create_attack_button(const std::string& filename)
    {
        gb::game_object_2d_shared_ptr button = gb::ces_entity::construct<gb::game_object_2d>();
        button->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        auto background = m_general_fabricator.lock()->create_sprite("ability.button.xml");
        background->size = glm::vec2(64.f);
        background->color = glm::u8vec4(255, 255, 0, 255);
        background->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        button->add_child(background);
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
        bound_touch_component->set_bounds(glm::vec4(0.f, 0.f,
                                                    64.f, 64.f));
        button->add_component(bound_touch_component);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_attack_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_fabricator::create_abitily_button(const std::string& filename)
    {
        gb::game_object_2d_shared_ptr button = gb::ces_entity::construct<gb::game_object_2d>();
        auto background = m_general_fabricator.lock()->create_sprite("ability.button.xml");
        background->size = glm::vec2(48.f);
        background->color = glm::u8vec4(0, 255, 255, 255);
        background->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        button->add_child(background);
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
        bound_touch_component->set_bounds(glm::vec4(0.f, 0.f,
                                                    48.f, 48.f));
        button->add_component(bound_touch_component);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_ability_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_fabricator::create_character_avatar_icon(const std::string& filename)
    {
        gb::game_object_2d_shared_ptr icon = gb::ces_entity::construct<gb::game_object_2d>();
        auto background = m_general_fabricator.lock()->create_sprite("ability.button.xml");
        background->size = glm::vec2(80.f);
        background->color = glm::u8vec4(255, 255, 255, 255);
        background->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        icon->add_child(background);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_character_avatar_icon);
        icon->add_component(ui_interaction_component);
    
        auto ui_avatar_icon_component = std::make_shared<ces_ui_avatar_icon_component>();
        icon->add_component(ui_avatar_icon_component);
        
        return icon;
    }
    
    gb::game_object_2d_shared_ptr gameplay_fabricator::create_opponent_avatar_icon(const std::string& filename)
    {
        gb::game_object_2d_shared_ptr icon = gb::ces_entity::construct<gb::game_object_2d>();
        auto background = m_general_fabricator.lock()->create_sprite("ability.button.xml");
        background->size = glm::vec2(80.f);
        background->color = glm::u8vec4(255, 255, 255, 255);
        background->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        icon->add_child(background);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_opponent_avatar_icon);
        icon->add_component(ui_interaction_component);
        
        auto ui_avatar_icon_component = std::make_shared<ces_ui_avatar_icon_component>();
        icon->add_component(ui_avatar_icon_component);
        
        return icon;
    }
}