//
//  ces_ui_quest_dialog_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_ui_quest_dialog_component : public gb::ces_base_component
    {
    private:
        
        i32 m_selected_quest_id;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_quest_dialog_component, gb::ces_base_component::g_guids_container)
        ces_ui_quest_dialog_component();
        ~ces_ui_quest_dialog_component() = default;
        
        void set_selected_quest_id(i32 id);
        i32 get_selected_quest_id() const;
        
        bool is_selected_quest_id_exist() const;
    };
};
