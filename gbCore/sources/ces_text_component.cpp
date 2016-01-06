//
//  ces_text_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_text_component.h"

namespace gb
{
    ces_text_component::ces_text_component() :
    m_is_text_changed(false),
    m_text("undefined")
    {
        m_type = e_ces_component_type_text;
    }
    
    ces_text_component::~ces_text_component()
    {
        
    }
    
    void ces_text_component::set_text(const std::string& text)
    {
        m_is_text_changed = true;
        m_text = text;
    }
    
    std::string ces_text_component::get_text() const
    {
        return m_text;
    }
    
    bool ces_text_component::is_text_changed() const
    {
        return m_is_text_changed;
    }
    
    void ces_text_component::reset()
    {
        m_is_text_changed = false;
    }
}