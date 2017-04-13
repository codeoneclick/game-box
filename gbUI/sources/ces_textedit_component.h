//
//  ces_textedit_component.h
//  gbUI
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    namespace ui
    {
        class ces_textedit_component : public ces_base_component
        {
        private:
            
        protected:
            
        public:
            
            CTTI_CLASS_GUID(ces_textedit_component, ces_base_component::g_guids_container)
            ces_textedit_component();
            ~ces_textedit_component();
        };
    };
};
