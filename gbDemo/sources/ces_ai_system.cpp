//
//  ces_ai_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ai_system.h"
#include "ces_ai_component.h"
#include "ces_transformation_component.h"
#include "std_extensions.h"
#include "glm_extensions.h"
#include "pathfinder.h"
#include "path_map.h"
#include "mesh.h"
#include "vbo.h"
#include "ibo.h"
#include "simple_shape.h"
#include "ai_move_action.h"
#include "ai_actions_processor.h"
#include "game_object.h"
#include "ces_character_controller_component.h"
#include "ces_light_mask_component.h"
#include "ces_transformation_component.h"
#include "ces_geometry_component.h"

namespace game
{
    ces_ai_system::ces_ai_system() :
    m_path_map(nullptr),
    m_pathfinder(std::make_shared<pathfinder>())
    {
        
    }
    
    ces_ai_system::~ces_ai_system()
    {
        
    }
    
    void ces_ai_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_ai_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_ai_system::update_recursively(entity, deltatime);
    }
    
    void ces_ai_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    bool ces_ai_system::line_rect_intersect(f32 x_0, f32 y_0, f32 x_1, f32 y_1, f32 left, f32 right, f32 top, f32 bottom)
    {
        f32 m = (y_1 - y_0) / (x_1 - x_0);
        f32 c = y_0 - (m * x_0);
        
        f32 top_intersection = 0.f;
        f32 bottom_intersection = 0.f;
        
        f32 toptrianglepoint = 0.f;
        f32 bottomtrianglepoint = 0.f;
        
        if(m > 0)
        {
            top_intersection = (m * left + c);
            bottom_intersection = (m * right + c);
        }
        else
        {
            top_intersection = (m * right + c);
            bottom_intersection = (m * left + c);
        }
        
        if(y_0 < y_1)
        {
            toptrianglepoint = y_0;
            bottomtrianglepoint = y_1;
        }
        else
        {
            toptrianglepoint = y_1;
            bottomtrianglepoint = y_0;
        }
        
        f32 topoverlap = 0.f;
        f32 botoverlap = 0.f;
        
        topoverlap = top_intersection > toptrianglepoint ? top_intersection : toptrianglepoint;
        botoverlap = bottom_intersection < bottomtrianglepoint ? bottom_intersection : bottomtrianglepoint;
        
        bool result = (topoverlap < botoverlap) && (!((botoverlap < top) || (topoverlap > bottom)));
        return result;
    }
    
    void ces_ai_system::update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        std::shared_ptr<ces_ai_component> ai_component = entity->get_component<ces_ai_component>();
        
        if(ai_component)
        {
            ai_actions_processor_shared_ptr actions_processor = ai_component->actions_processor;
            actions_processor->update(deltatime);
            
            if(!m_main_character.expired())
            {
                auto main_character = m_main_character.lock();
                if(main_character != entity)
                {
                    bool is_intersected = false;
                    auto ai_transformation_component = entity->get_component<gb::ces_transformation_component>();
                    
                    gb::mesh_shared_ptr light_source_mesh = nullptr;
                    gb::mesh_shared_ptr ai_body_mesh = nullptr;

                    std::list<gb::ces_entity_shared_ptr> children = main_character->children;
                    for(const auto& child : children)
                    {
                        std::string part_name = child->tag;
                        if(part_name == "character")
                        {
                            std::list<gb::ces_entity_shared_ptr> children = child->children;
                            for(const auto& child : children)
                            {
                                std::string part_name = child->tag;
                                if(part_name == "light_source")
                                {
                                    gb::game_object_shared_ptr light_source = std::static_pointer_cast<gb::game_object>(child);
                                    auto light_mask_component = light_source->get_component<gb::ces_light_mask_component>();
                                    light_source_mesh = light_mask_component->get_mesh();
                                }
                            }
                        }
                    }
                    
                    children = entity->children;
                    for(const auto& child : children)
                    {
                        std::string part_name = child->tag;
                        if(part_name == "character")
                        {
                            std::list<gb::ces_entity_shared_ptr> children = child->children;
                            for(const auto& child : children)
                            {
                                std::string part_name = child->tag;
                                if(part_name == "body")
                                {
                                    auto geometry_component = child->get_component<gb::ces_geometry_component>();
                                    ai_body_mesh = geometry_component->get_mesh();
                                }
                            }
                        }
                    }
                    
                    if(light_source_mesh && ai_body_mesh)
                    {
                        is_intersected = gb::simple_shape::intersection(ai_body_mesh->get_vbo(), ai_body_mesh->get_ibo(), ai_transformation_component->get_matrix_m(),
                                                                        light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f));
                    }
                    
                    entity->visible = is_intersected;
                    if(!is_intersected)
                    {
                        std::cout<<"invisible"<<std::endl;
                    }
                }
            }
            
            if(!actions_processor->is_actions_exist())
            {
                glm::ivec2 goal_position_index;
                goal_position_index.x = std::get_random_i(0, m_path_map->get_size().x - 1);
                goal_position_index.y = std::get_random_i(0, m_path_map->get_size().y - 1);
                
                auto transformation_component = entity->get_component<gb::ces_transformation_component>();
                glm::vec2 current_position = transformation_component->get_position();
                glm::ivec2 current_position_index;
                current_position_index.x = std::max(0, std::min(static_cast<i32>(current_position.x / m_path_map->get_cell_size().x), m_path_map->get_size().x - 1));
                current_position_index.y = std::max(0, std::min(static_cast<i32>(current_position.y / m_path_map->get_cell_size().y), m_path_map->get_size().y - 1));
                
                std::vector<std::shared_ptr<astar_node>> path;
                m_pathfinder->set_start(m_path_map->get_path_node(current_position_index.x, current_position_index.y));
                m_pathfinder->set_goal(m_path_map->get_path_node(goal_position_index.x, goal_position_index.y));
                
                bool is_found = m_pathfinder->find_path(path);
                if(is_found && path.size() > 1)
                {
                    path.resize(path.size() - 1);
                    std::reverse(path.begin(), path.end());
                    
                    for(const auto& point : path)
                    {
                        ai_move_action_shared_ptr move_action = std::make_shared<ai_move_action>();
                        glm::vec2 goal_position;
                        goal_position.x = static_cast<f32>(point->get_x()) * static_cast<f32>(m_path_map->get_cell_size().x) + static_cast<f32>(m_path_map->get_cell_size().x) * .5f;
                        goal_position.y = static_cast<f32>(point->get_y()) * static_cast<f32>(m_path_map->get_cell_size().y) + static_cast<f32>(m_path_map->get_cell_size().y) * .5f;
                        move_action->set_parameters(std::static_pointer_cast<gb::game_object>(entity), goal_position);
                        actions_processor->add_action(move_action);
                    }
                }
            }
        }
        
        auto character_controller_component = entity->get_component<ces_character_controller_component>();
        if(character_controller_component)
        {
            if(character_controller_component->mode == ces_character_controller_component::e_mode::ai)
            {
                std::string character_key = entity->tag;
                m_ai_characters[character_key] = entity;
            }
            else if(character_controller_component->mode == ces_character_controller_component::e_mode::main)
            {
                m_main_character = entity;
            }
        }
        
        std::list<gb::ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_ai_system::update_recursively(child, deltatime);
        }
    }
    
    void ces_ai_system::set_path_map(const path_map_shared_ptr& path_map)
    {
        m_path_map = path_map;
    }
}
