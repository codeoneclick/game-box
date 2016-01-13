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
#include "ces_transformation_component.h"

namespace gb
{
    static const i32 k_min_font_height = 1;
    static const i32 k_max_font_height = 32;
    
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
    
    void label::set_font_height(i32 height)
    {
        height = std::min(k_max_font_height, std::max(k_min_font_height, height));
        unsafe_get_transformation_component_from_this->set_scale(glm::vec2(static_cast<f32>(height) / static_cast<f32>(k_max_font_height)));
    }
}