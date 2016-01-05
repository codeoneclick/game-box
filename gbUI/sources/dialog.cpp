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
        
        dialog::dialog() :
        m_position(glm::ivec2(0)),
        m_size(glm::ivec2(2))
        {
            
        }
        
        dialog::~dialog()
        {
            
        }
        
        void dialog::set_position(const glm::ivec2& position)
        {
            m_position = position;
        }
        
        glm::ivec2 dialog::get_position() const
        {
            return m_position;
        }
        
        void dialog::set_size(const glm::ivec2& size)
        {
            m_size = size;
        }
        
        glm::ivec2 dialog::get_size() const
        {
            return m_size;
        }
        
        const std::vector<element_shared_ptr>& dialog::get_elements() const
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