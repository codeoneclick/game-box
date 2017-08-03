//
//  ces_ui_interaction_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_interaction_component.h"

namespace game
{
    const i32 ces_ui_interaction_component::k_quest_dialog_accept_button = 1;
    const i32 ces_ui_interaction_component::k_quest_dialog_decline_button = 2;
    
    ces_ui_interaction_component::ces_ui_interaction_component() :
    m_type(e_type_unknown)
    {
        
    }
    
    ces_ui_interaction_component::e_type ces_ui_interaction_component::get_type() const
    {
        return m_type;
    }
    
    void ces_ui_interaction_component::set_type(e_type type)
    {
        m_type = type;
    }
}
