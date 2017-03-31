//
//  ai_attack_action.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ai_attack_action.h"
#include "game_object.h"
#include "ces_transformation_component.h"
#include "ces_light_mask_component.h"
#include "ces_geometry_component.h"
#include "ces_box2d_body_component.h"
#include "glm_extensions.h"
#include "ai_attack_move_action.h"

namespace game
{
    ai_attack_action::ai_attack_action() :
    m_shoot_timeinterval(100),
    m_last_shoot_deltatime(0),
    m_shoot_distance(0.f),
    m_move_bounds(0)
    {
        target.getter([=]() {
            return m_target.lock();
        });
    }
    
    ai_attack_action::~ai_attack_action()
    {
        
    }
    
    void ai_attack_action::set_parameters(const gb::game_object_shared_ptr& executor,
                                          const gb::game_object_shared_ptr& target,
                                          f32 shoot_distance,
                                          const glm::ivec4& move_bounds)
    {
        m_executor = executor;
        m_target = target;
        m_shoot_distance = shoot_distance;
        m_move_bounds = move_bounds;
    }
    
    void ai_attack_action::update(f32 deltatime)
    {
        m_last_shoot_deltatime -= deltatime * 1000;
        if(!m_executor.expired() && !m_target.expired())
        {
            if(m_state != e_ai_action_state_ended && m_state != e_ai_action_state_interrupted)
            {
                if(m_state == e_ai_action_state_none)
                {
                    m_state = e_ai_action_state_in_progress;
                    if(m_start_callback)
                    {
                        m_start_callback(shared_from_this());
                    }
                }
                
                auto executor = m_executor.lock();
                auto target = m_target.lock();
                
                auto executor_transformation_component = executor->get_component<gb::ces_transformation_component>();
                auto target_transformation_component = target->get_component<gb::ces_transformation_component>();
                
                glm::vec2 executor_position = executor_transformation_component->get_position();
                glm::vec2 target_position = target_transformation_component->get_position();
                f32 distance = glm::distance(executor_position, target_position);
                
                if(distance <= m_shoot_distance)
                {
                    gb::ces_entity_shared_ptr light_source_entity = executor->get_child("light_source", true);
                    gb::mesh_shared_ptr light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                    
                    gb::ces_entity_shared_ptr body_entity = target->get_child("body", true);
                    gb::mesh_shared_ptr body_mesh = body_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                    
                    glm::vec2 direction = glm::normalize(target_position - executor_position);
                    f32 goal_rotation = atan2f(direction.x, -direction.y);
                    goal_rotation = glm::wrap_degrees(glm::degrees(goal_rotation));
                    executor->rotation = goal_rotation;
                    executor->position = executor_position;
                    
                    gb::ces_box2d_body_component_shared_ptr box2d_body_component =
                    executor->get_component<gb::ces_box2d_body_component>();
                    box2d_body_component->velocity = glm::vec2(0.f);
                    
                    if(light_source_mesh && body_mesh)
                    {
                        std::vector<glm::triangle> light_source_triangles;
                        if(gb::mesh::intersect(body_mesh->get_vbo(), body_mesh->get_ibo(), target_transformation_component->get_matrix_m(), true,
                                               light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false, nullptr, &light_source_triangles))
                        {
                            if(m_sub_actions.empty())
                            {
                                glm::vec2 random_position_v[2];
                                random_position_v[0].x = std::get_random_i(glm::clamp(static_cast<i32>(target_position.x) - static_cast<i32>(m_shoot_distance * .75f), m_move_bounds.x, m_move_bounds.z),
                                                                           glm::clamp(static_cast<i32>(target_position.x) - static_cast<i32>(m_shoot_distance * .5f),  m_move_bounds.y, m_move_bounds.w));
                                random_position_v[0].y = std::get_random_i(glm::clamp(static_cast<i32>(target_position.y) - static_cast<i32>(m_shoot_distance * .75f), m_move_bounds.x, m_move_bounds.z),
                                                                           glm::clamp(static_cast<i32>(target_position.y) - static_cast<i32>(m_shoot_distance * .5f),  m_move_bounds.y, m_move_bounds.w));
                                random_position_v[1].x = std::get_random_i(glm::clamp(static_cast<i32>(target_position.x) + static_cast<i32>(m_shoot_distance * .5f),  m_move_bounds.x, m_move_bounds.z),
                                                                           glm::clamp(static_cast<i32>(target_position.x) + static_cast<i32>(m_shoot_distance * .75f), m_move_bounds.y, m_move_bounds.w));
                                random_position_v[1].y = std::get_random_i(glm::clamp(static_cast<i32>(target_position.y) + static_cast<i32>(m_shoot_distance * .5f),  m_move_bounds.x, m_move_bounds.z),
                                                                           glm::clamp(static_cast<i32>(target_position.y) + static_cast<i32>(m_shoot_distance * .75f), m_move_bounds.y, m_move_bounds.w));
                                
                                glm::vec2 random_position;
                                random_position.x = random_position_v[std::get_random_i(0, 1)].x;
                                random_position.y = random_position_v[std::get_random_i(0, 1)].y;
                                
                                for(const auto& triangle : light_source_triangles)
                                {
                                    if(glm::point_in_triangle(random_position, triangle))
                                    {
                                        ai_attack_move_action_shared_ptr move_action = std::make_shared<ai_attack_move_action>();
                                        glm::vec2 goal_position;
                                        move_action->set_parameters(executor, random_position);
                                        ai_attack_action::add_sub_action(move_action);
                                        break;
                                    }
                                }
                            }
                            if(m_last_shoot_deltatime <= 0)
                            {
                                m_last_shoot_deltatime = m_shoot_timeinterval;
                                if(m_in_progress_callback)
                                {
                                    m_in_progress_callback(shared_from_this());
                                }
                            }
                        }
                        else
                        {
                            m_state = e_ai_action_state_ended;
                        }
                    }
                    else
                    {
                        assert(false);
                    }
                }
                else
                {
                    m_state = e_ai_action_state_ended;
                }
            }
        }
        else
        {
            assert(false);
        }
    }
}
