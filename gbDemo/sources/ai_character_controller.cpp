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
#include "animated_sprite.h"
#include "ai_move_action.h"
#include "ai_actions_processor.h"
#include "glm_extensions.h"
#include "ces_box2d_body_component.h"
#include "ces_character_controller_component.h"

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
        
        std::shared_ptr<ces_character_controller_component> character_controller_component = std::make_shared<game::ces_character_controller_component>();
        character_controller_component->mode = ces_character_controller_component::e_mode::ai;
        ai_character_controller::add_component(character_controller_component);
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
            if(action->instance_guid() == ai_move_action::class_guid())
            {
                if(!action->is_progress_callback_exist())
                {
                    action->set_in_progress_callback([this](const ai_action_shared_ptr& action) {
                        std::list<gb::ces_entity_shared_ptr> children = m_character->children;
                        for(const auto& child : children)
                        {
                            std::string part_name = child->tag;
                            if(part_name == "feet" || part_name == "body")
                            {
                                gb::anim::animated_sprite_shared_ptr part = std::static_pointer_cast<gb::anim::animated_sprite>(child);
                                part->goto_and_play("move");
                            }
                        }
                    });
                }
                if(!action->is_end_callback_exist())
                {
                    action->set_end_callback([this](const ai_action_shared_ptr& action) {
                        std::list<gb::ces_entity_shared_ptr> children = m_character->children;
                        for(const auto& child : children)
                        {
                            std::string part_name = child->tag;
                            if(part_name == "feet" || part_name == "body")
                            {
                                gb::anim::animated_sprite_shared_ptr part = std::static_pointer_cast<gb::anim::animated_sprite>(child);
                                part->goto_and_play("idle");
                            }
                        }
                    });
                }
            }
        }
    }
}

