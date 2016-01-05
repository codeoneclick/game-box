//
//  control.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "control.h"
namespace gb
{
    namespace ui
    {
        control::control() :
        m_position(glm::ivec2(0)),
        m_size(glm::ivec2(2))
        {
            
        }
        
        control::~control()
        {
            
        }
        
        void control::set_position(const glm::ivec2& position)
        {
            m_position = position;
        }
        
        glm::ivec2 control::get_position() const
        {
            return m_position;
        }
        
        void control::set_size(const glm::ivec2& size)
        {
            m_size = size;
        }
        
        glm::ivec2 control::get_size() const
        {
            return m_size;
        }
        
        const std::vector<element_shared_ptr>& control::get_elements() const
        {
            return m_elements;
        }
    }
}