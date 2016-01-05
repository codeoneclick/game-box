//
//  ai_character_controller.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/15/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ai_character_controller.h"
#include "game_object.h"
#include "level.h"
#include "ai_actions_processor.h"
#include "ai_move_action.h"
#include "model3d_animated.h"
#include "glm_extensions.h"
#include "game_object_navigator.h"

namespace koth
{
    f32 ai_character_controller::m_trashhold_distance = 2.f;
    
    ai_character_controller::ai_character_controller(const gb::game_object_shared_ptr& game_object,
                                                     const level_shared_ptr& level) :
    koth::character_controller(game_object, nullptr),
    m_goal_position_index(glm::ivec2(-1)),
    m_level(level)
    {
        m_pathfinder = std::make_shared<pathfinder>();
        m_actions_processor = std::make_shared<ai_actions_processor>();
    }
    
    ai_character_controller::~ai_character_controller()
    {
        
    }
    
    void ai_character_controller::update(f32 deltatime)
    {
        m_actions_processor->update(deltatime);
        
        f32 distance = glm::distance(m_game_object->get_position(), m_goal_position);
        
        if(!m_actions_processor->is_actions_exist() && distance > m_trashhold_distance &&
           m_goal_position_index.x != -1 && m_goal_position_index.y != -1)
        {
            std::vector<std::shared_ptr<astar_node>> path;
            
            glm::ivec2 current_position_index;
            current_position_index.x = std::max(0, std::min(static_cast<i32>(m_game_object->get_position().x), m_level->get_size().x - 1));
            current_position_index.y = std::max(0, std::min(static_cast<i32>(m_game_object->get_position().z), m_level->get_size().y - 1));
            
            m_pathfinder->set_start(m_level->get_path_node(current_position_index.x, current_position_index.y));
            m_pathfinder->set_goal(m_level->get_path_node(m_goal_position_index.x, m_goal_position_index.y));
            
            bool is_found = m_pathfinder->find_path(path);
            if(is_found)
            {
                if(path.size() > 1)
                {
                    path.resize(path.size() - 1);
                    std::reverse(path.begin(), path.end());
                    
                    glm::vec3 previous_position = m_game_object->get_position();
                    for(const auto& point : path)
                    {
                        ai_move_action_shared_ptr move_action = std::make_shared<ai_move_action>();
                        move_action->set_parameters(previous_position, glm::vec3(point->get_x(),
                                                                                 m_game_object->get_position().y,
                                                                                 point->get_y()));
                        
                        previous_position = glm::vec3(point->get_x(),
                                                      m_game_object->get_position().y,
                                                      point->get_y());
                        
                        move_action->set_in_progress_callback([this](const ai_action_shared_ptr& action) {
                            
                            ai_move_action_shared_ptr move_action = std::static_pointer_cast<ai_move_action>(action);
                            
                            f32 current_angle = glm::wrap_degrees(m_game_object->get_rotation().y);
                            f32 goal_angle =  glm::wrap_degrees(glm::degrees(move_action->get_rotation()));
                            
                            character_controller::set_rotation(glm::vec3(.0f, glm::mix_angles_degrees(current_angle, goal_angle, .1f), .0f));
                            character_controller::set_position(move_action->get_position());
                        });
                        m_actions_processor->add_action(move_action);
                        m_current_actions.push_back(move_action);
                    }
                }
            }
        }
        
        gb::model3d_animated_shared_ptr animated_model = std::static_pointer_cast<gb::model3d_animated>(m_game_object);
        if(m_actions_processor->is_actions_exist())
        {
            animated_model->set_animation("RUN");
        }
        else
        {
            if(m_goal_position != m_game_object->get_position())
            {
                f32 current_angle = glm::wrap_degrees(m_game_object->get_rotation().y);
                glm::vec3 direction = glm::normalize(m_goal_position - m_game_object->get_position());
                f32 goal_angle =  glm::wrap_degrees(glm::degrees(atan2f(direction.x, direction.z)));
                character_controller::set_rotation(glm::vec3(.0f, glm::mix_angles_degrees(current_angle, goal_angle, .1f), .0f));
            }
            
            animated_model->set_animation("IDLE");
        }
        m_game_object_navigator->update(deltatime);
    }
    
    void ai_character_controller::set_goal_position(const glm::vec3& position)
    {
        m_goal_position = position;
        m_goal_position_index.x = std::max(0, std::min(static_cast<i32>(position.x), m_level->get_size().x - 1));
        m_goal_position_index.y = std::max(0, std::min(static_cast<i32>(position.z), m_level->get_size().y - 1));
    }
    
    void ai_character_controller::set_position(const glm::vec3& position)
    {
        character_controller::set_position(position);
    }
}