//
//  ces_textedit_component.cpp
//  gbUI
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_textedit_component.h"

namespace gb
{
    namespace ui
    {
        ces_textedit_component::ces_textedit_component() :
        m_on_focus_changed_callback(nullptr),
        m_on_text_changed_callback(nullptr),
        m_on_backspace_callback(nullptr)
        {
            focus.setter([=](bool value) {
               if(m_on_focus_changed_callback)
               {
                   m_on_focus_changed_callback(value);
               }
            });
            
            focus.getter([=]() {
                assert(false);
                return false;
            });
        }
        
        ces_textedit_component::~ces_textedit_component()
        {
            
        }
        
        void ces_textedit_component::on_text_changed(const std::string& symbol)
        {
            if(m_on_text_changed_callback)
            {
                m_on_text_changed_callback(symbol);
            }
        }
        
        void ces_textedit_component::on_backspace()
        {
            if(m_on_backspace_callback)
            {
                m_on_backspace_callback();
            }
        }
        
        void ces_textedit_component::set_on_focus_changed_callback(const on_focus_changed_callback_t& callback)
        {
            m_on_focus_changed_callback = callback;
        }
        
        void ces_textedit_component::set_on_text_changed_callback(const on_text_changed_callback_t& callback)
        {
            m_on_text_changed_callback = callback;
        }
        
        void ces_textedit_component::set_on_backspace_callback(const on_backspace_callback_t& callback)
        {
            m_on_backspace_callback = callback;
        }
    }
}
