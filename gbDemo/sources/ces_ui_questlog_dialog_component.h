//
//  ces_ui_questlog_dialog_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_ui_questlog_dialog_component : public gb::ces_base_component
    {
    private:
        
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_questlog_dialog_component, gb::ces_base_component::g_guids_container)
        ces_ui_questlog_dialog_component();
        ~ces_ui_questlog_dialog_component() = default;
    };
};

