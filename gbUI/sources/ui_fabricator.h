//
//  ui_fabricator.h
//  gbUI
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ui_fabricator_h
#define ui_fabricator_h

#include "main_headers.h"
#include "declarations.h"
#include "ui_declarations.h"

namespace gb
{
    namespace ui
    {
        class ui_fabricator
        {
        private:
            
        protected:
            
            std::set<control_shared_ptr> m_controls_container;
            
            scene_fabricator_shared_ptr m_fabricator;
            
        public:
            
            ui_fabricator(const scene_fabricator_shared_ptr& fabricator);
            ~ui_fabricator();
            
            button_shared_ptr create_button(const glm::vec2& size, std::function<void(const button_shared_ptr&)> callback);
        };
    };
};

#endif
