//
//  sprite.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "sprite.h"
#include "ces_geometry_quad_component.h"

namespace gb
{
    sprite::sprite()
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_quad_component>();
        ces_entity::add_component(geometry_component);
    }
    
    sprite::~sprite()
    {
        
    }
    
    void sprite::set_size(const glm::vec2& size)
    {
        unsafe_get_geometry_quad_component_from_this->set_size(size);
    }
    
    glm::vec2 sprite::get_size() const
    {
        return unsafe_get_geometry_quad_component_from_this->get_size();
    }
    
    void sprite::set_pivot(const glm::vec2& pivot)
    {
        unsafe_get_geometry_quad_component_from_this->set_pivot(pivot);
    }
    
    glm::vec2 sprite::get_pivot() const
    {
        return unsafe_get_geometry_quad_component_from_this->get_pivot();
    }
}