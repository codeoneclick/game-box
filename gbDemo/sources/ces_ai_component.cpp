//
//  ces_ai_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ai_component.h"
#include "ai_actions_processor.h"

namespace game
{
    ces_ai_component::ces_ai_component() :
    m_goal_position(glm::vec2(0.f)),
    m_goal_position_index(glm::ivec2(0)),
    m_actions_processor(std::make_shared<ai_actions_processor>())
    {
        goal_position.setter([=](const glm::vec2& position) {
            m_goal_position = position;
        });
        
        goal_position.getter([=]() {
            return m_goal_position;
        });
        
        goal_position_index.setter([=](const glm::ivec2& position) {
            m_goal_position_index = position;
        });
        
        goal_position_index.getter([=]() {
            return m_goal_position_index;
        });
        
        actions_processor.getter([=]() {
            return m_actions_processor;
        });
    }
    
    ces_ai_component::~ces_ai_component()
    {
        
    }
}
