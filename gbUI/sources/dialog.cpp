//
//  dialog.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "dialog.h"

namespace gb
{
    namespace ui
    {
        
        dialog::dialog()
        {
            
        }
        
        dialog::~dialog()
        {
            
        }
        
        const std::vector<sprite_shared_ptr>& dialog::get_elements() const
        {
            return m_elements;
        }
        
        const std::map<i32, control_shared_ptr>& dialog::get_controls() const
        {
            return m_controls;
        }
        
        void dialog::add_control(const control_shared_ptr& control, i32 id)
        {
            const auto& iterator = m_controls.find(id);
            if(iterator == m_controls.end())
            {
                m_controls.insert(std::make_pair(id, control));
            }
            else
            {
                assert(false);
            }
        }
        
        void dialog::remove_control(i32 id)
        {
            const auto& iterator = m_controls.find(id);
            if(iterator != m_controls.end())
            {
                m_controls.erase(iterator);
            }
            else
            {
                assert(false);
            }
        }
        
        control_shared_ptr dialog::get_control(i32 id) const
        {
            const auto& iterator = m_controls.find(id);
            if(iterator != m_controls.end())
            {
                return iterator->second;
            }
            else
            {
                assert(false);
                return nullptr;
            }
        }
    }
}