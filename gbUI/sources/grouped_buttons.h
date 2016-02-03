//
//  radio_button.h
//  gbUI
//
//  Created by Serhii Serhiiv on 2/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef grouped_buttons_h
#define grouped_buttons_h

#include "control.h"

namespace gb
{
    namespace ui
    {
        class grouped_buttons : public control
        {
        public:
            
           typedef std::function<void(i32, const ces_entity_shared_ptr&)> t_on_pressed_callback;
            
        private:
            
        protected:
            
            t_on_pressed_callback m_on_pressed_callback;
            f32 m_separator_offset;
            
            std::vector<button_shared_ptr> m_buttons;
            
            void on_button_pressed(const ces_entity_shared_ptr& entity);
            
        public:
            
            grouped_buttons(const scene_fabricator_shared_ptr& fabricator);
            ~grouped_buttons();
            
            void create();
            
            void set_size(const glm::vec2& size);
            
            void set_separator_offset(f32 separator_offset);
            
            void set_on_pressed_callback(const t_on_pressed_callback& callback);
            
            void set_data_source(const std::vector<std::string>& data_source);
        };
    };
};


#endif
