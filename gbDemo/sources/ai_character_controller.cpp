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
    
    void ai_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
    }
}

