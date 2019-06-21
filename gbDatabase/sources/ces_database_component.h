//
//  ces_database_component.h
//  gb_sound
//
//  Created by serhii serhiiv on 6/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    namespace al
    {
        class ces_database_component : public ces_base_component
        {
        private:
            
        protected:
            
        public:
            
            STTI_CLASS_GUID(ces_database_component, ces_base_component::g_guids_container)
            ces_database_component();
            ~ces_database_component();
        };
    };
};

