//
//  ai_attack_action.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ai_attack_action.h"
#include "game_object_2d.h"
#include "ces_transformation_2d_component.h"
#include "ces_light_mask_component.h"
#include "ces_geometry_component.h"
#include "ces_box2d_body_component.h"
#include "ces_character_statistic_component.h"
#include "glm_extensions.h"
#include "character.h"

namespace game
{
    ai_attack_action::ai_attack_action(const gb::ces_entity_shared_ptr& owner) :
    game::ai_action(owner),
    m_last_attack_deltatime(0),
    m_attack_distance(0.f)
    {
        target.getter([=]() {
            return m_target.lock();
        });
    }
    
    ai_attack_action::~ai_attack_action()
    {
        
    }
    
    void ai_attack_action::set_parameters(const gb::game_object_2d_shared_ptr& executor,
                                          const gb::game_object_2d_shared_ptr& target,
                                          f32 attack_distance)
    {
        m_executor = executor;
        m_target = target;
        m_attack_distance = attack_distance;
    }
    
    void ai_attack_action::update(f32 dt)
    {
        m_last_attack_deltatime -= dt * 1000;
        if(!m_executor.expired() && !m_target.expired())
        {
            auto executor = m_executor.lock();
            auto target = m_target.lock();
            auto target_character_statistic_component = target->get_component<ces_character_statistic_component>();
            auto executor_character_statistic_component = executor->get_component<ces_character_statistic_component>();
            if(target_character_statistic_component->is_dead)
            {
                m_state = e_ai_action_state_ended;
            }
            
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
                
                auto executor_transformation_component = executor->get_component<gb::ces_transformation_2d_component>();
                auto target_transformation_component = target->get_component<gb::ces_transformation_2d_component>();
                
                glm::vec2 executor_position = executor_transformation_component->get_position();
                glm::vec2 target_position = target_transformation_component->get_position();
                f32 distance = glm::distance(executor_position, target_position);
                
                if(distance <= m_attack_distance)
                {
                    gb::ces_entity_shared_ptr light_source_entity = executor->get_child("");//character::parts::k_light_source_part, true);
                    gb::mesh_2d_shared_ptr light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                    
                    gb::ces_entity_shared_ptr bounds_entity = target->get_child("");//character::parts::k_bounds_part, true);
                    gb::mesh_2d_shared_ptr bounds_mesh = bounds_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                    
                    glm::vec2 direction = glm::normalize(target_position - executor_position);
                    f32 goal_rotation = atan2f(direction.x, -direction.y);
                    goal_rotation = glm::wrap_degrees(glm::degrees(goal_rotation));
                    executor->rotation = goal_rotation;
                    executor->position = executor_position;
                    
                    gb::ces_box2d_body_component_shared_ptr box2d_body_component =
                    executor->get_component<gb::ces_box2d_body_component>();
                    box2d_body_component->velocity = glm::vec2(0.f);
                    
                    if(light_source_mesh && bounds_mesh)
                    {
                        std::vector<glm::triangle> light_source_triangles;
                        if(gb::mesh_2d::intersect(bounds_mesh->get_vbo(), bounds_mesh->get_ibo(), target_transformation_component->get_matrix_m(), true, light_source_mesh->get_vbo(), light_source_mesh->get_ibo(), glm::mat4(1.f), false, nullptr, &light_source_triangles))
                        {
                            if(m_last_attack_deltatime <= 0)
                            {
                                m_last_attack_deltatime = executor_character_statistic_component->current_attack_speed;
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
