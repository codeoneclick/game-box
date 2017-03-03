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
            
            scene_fabricator_shared_ptr m_fabricator;
            
        public:
            
            ui_fabricator(const scene_fabricator_shared_ptr& fabricator);
            ~ui_fabricator();
            
            button_shared_ptr create_button(const glm::vec2& size, std::function<void(const ces_entity_shared_ptr&)> callback);
            grouped_buttons_shared_ptr create_grouped_buttons(const glm::vec2& size,
                                                              std::function<void(i32, const ces_entity_shared_ptr&)> callback);
            table_view_shared_ptr create_table_view(const glm::vec2& size);
            tree_view_shared_ptr create_tree_view(const glm::vec2& size);
            content_tab_list_shared_ptr create_content_tab_list(const glm::vec2& size);
            switcher_shared_ptr create_switcher(const glm::vec2& size);
            joystick_shared_ptr create_joystick(const glm::vec2& size);
            fullscreen_joystick_shared_ptr create_fullscreen_joystick(const glm::vec2& size, const glm::vec2& center);
            console_shared_ptr create_console(const glm::vec2& size, i32 lines_count);
        };
    };
};

#endif
