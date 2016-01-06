//
//  label.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "label.h"
#include "ces_geometry_freeform_component.h"
#include "ces_text_component.h"

namespace gb
{
    label::label()
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_freeform_component>();
        ces_entity::add_component(geometry_component);
        
        ces_text_component_shared_ptr text_component = std::make_shared<ces_text_component>();
        ces_entity::add_component(text_component);
    }
    
    label::~label()
    {
        
    }
    
    void label::set_text(const std::string &text)
    {
        unsafe_get_text_component_from_this->set_text(text);
    }
    
    std::string label::get_text() const
    {
        return unsafe_get_text_component_from_this->get_text();
    }
}