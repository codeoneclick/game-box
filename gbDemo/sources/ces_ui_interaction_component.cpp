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
    const i32 ces_ui_interaction_component::k_levels_list_dialog_levels_table = 1;
    const i32 ces_ui_interaction_component::k_cars_list_dialog_table = 1;
    
    ces_ui_interaction_component::ces_ui_interaction_component() :
    m_ui(e_ui_unknown)
    {
        
    }
    
    ces_ui_interaction_component::e_ui ces_ui_interaction_component::get_ui() const
    {
        return m_ui;
    }
    
    void ces_ui_interaction_component::set_ui(e_ui ui)
    {
        m_ui = ui;
    }
}
