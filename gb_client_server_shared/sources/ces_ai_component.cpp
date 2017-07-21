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
    m_actions_processor(std::make_shared<ai_actions_processor>())
    {
        
        actions_processor.getter([=]() {
            return m_actions_processor;
        });
    }
    
    ces_ai_component::~ces_ai_component()
    {
        
    }
}
