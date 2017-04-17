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
        public:
            
            typedef std::function<void(bool)> on_focus_changed_callback_t;
            typedef std::function<void(const std::string&)> on_text_changed_callback_t;
            typedef std::function<void(void)> on_backspace_callback_t;
        
        private:
            
        protected:
            
            on_focus_changed_callback_t m_on_focus_changed_callback;
            on_text_changed_callback_t m_on_text_changed_callback;
            on_backspace_callback_t m_on_backspace_callback;
            
        public:
            
            CTTI_CLASS_GUID(ces_textedit_component, ces_base_component::g_guids_container)
            ces_textedit_component();
            ~ces_textedit_component();
            
            std::property_rw<bool> focus;
            void on_text_changed(const std::string& symbol);
            void on_backspace();
            
            void set_on_focus_changed_callback(const on_focus_changed_callback_t& callback);
            void set_on_text_changed_callback(const on_text_changed_callback_t& callback);
            void set_on_backspace_callback(const on_backspace_callback_t& callback);
        };
    };
};
