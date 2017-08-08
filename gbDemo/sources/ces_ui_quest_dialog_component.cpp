//
//  ces_ui_quest_dialog_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_quest_dialog_component.h"

namespace game
{
    ces_ui_quest_dialog_component::ces_ui_quest_dialog_component() :
    m_selected_quest_id(-1)
    {
        
    }
    
    void ces_ui_quest_dialog_component::set_selected_quest_id(i32 id)
    {
        m_selected_quest_id = id;
    }
    
    i32 ces_ui_quest_dialog_component::get_selected_quest_id() const
    {
        return m_selected_quest_id;
    }
    
    bool ces_ui_quest_dialog_component::is_selected_quest_id_exist() const
    {
        return m_selected_quest_id != -1;
    }
}
