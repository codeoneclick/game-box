//
//  ces_ui_control_component.h
//  gbUI
//
//  Created by serhii serhiiv on 8/18/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    namespace ui
    {
        class ces_ui_control_component : public ces_base_component
        {
        private:
            
        protected:
            
        public:
            
            CTTI_CLASS_GUID(ces_ui_control_component, ces_base_component::g_guids_container)
            ces_ui_control_component();
            ~ces_ui_control_component();
        };
    };
};

