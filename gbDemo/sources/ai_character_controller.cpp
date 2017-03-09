//
//  ai_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ai_character_controller.h"
#include "ces_action_component.h"
#include "ces_ai_component.h"
#include "character.h"
#include "ai_move_action.h"
#include "ai_actions_processor.h"
#include "glm_extensions.h"
#include "ces_box2d_body_component.h"

namespace game
{
    ai_character_controller::ai_character_controller(const gb::game_object_shared_ptr& layer,
                                                     const gb::scene_graph_shared_ptr& scene_graph,
                                                     const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                     const gb::anim::anim_fabricator_shared_ptr& anim_fabricator) :
    m_layer(layer),
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator),
    m_anim_fabricator(anim_fabricator)
    {
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&ai_character_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        ai_character_controller::add_component(action_component);
        
        ces_ai_component_shared_ptr ai_component = std::make_shared<ces_ai_component>();
        ai_character_controller::add_component(ai_component);
    }
    
    ai_character_controller::~ai_character_controller()
    {
        
    }
    
    void ai_character_controller::setup(const std::string& filename)
    {
        m_character = std::make_shared<character>();
        std::static_pointer_cast<character>(m_character)->setup(filename,
                                                                m_scene_graph.lock(),
                                                                m_scene_fabricator.lock(),
                                                                m_anim_fabricator.lock());
        ai_character_controller::add_child(m_character);
    }
    
#define k_move_speed -1000.f
#define k_rotate_speed 100.f
    
    void ai_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        ai_character_controller::get_component<gb::ces_box2d_body_component>();

        auto ai_component = ai_character_controller::get_component<ces_ai_component>();
        ai_actions_processor_shared_ptr actions_processor = ai_component->actions_processor;
        
        auto action = actions_processor->top_action();
        if(action)
        {
            action->set_in_progress_callback([this, deltatime, box2d_body_component](const ai_action_shared_ptr& action) {
                
                ai_move_action_shared_ptr move_action = std::static_pointer_cast<ai_move_action>(action);
                
                f32 current_rotation = ai_character_controller::rotation;
                current_rotation = glm::wrap_degrees(current_rotation);
                f32 goal_rotation = glm::wrap_degrees(glm::degrees(move_action->get_rotation()));
                
                glm::vec2 current_position = ai_character_controller::position;
                glm::vec2 goal_position = move_action->get_position() * 16.f;
                
                //glm::vec2 direction = glm::normalize(goal_position - current_position);
                
                f32 current_move_speed = k_move_speed * deltatime;
                
                glm::vec2 velocity = glm::vec2(-sinf(glm::radians(goal_rotation)) * current_move_speed,
                                               cosf(glm::radians(goal_rotation)) * current_move_speed);
                
                ai_character_controller::position = current_position;
                ai_character_controller::rotation = current_rotation;
                box2d_body_component->velocity = velocity;

                
                //ai_character_controller::rotation = glm::mix_angles_degrees(current_rotation, goal_rotation, .001f);
                //ai_character_controller::position = glm::mix(current_position, goal_position, .1f);
            });
        }
    }
}

