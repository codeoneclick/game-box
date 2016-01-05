//
//  renderable_game_object.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/10/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "renderable_game_object.h"
#include "ces_material_component.h"
#include "ces_geometry_component.h"

namespace gb
{
    renderable_game_object::renderable_game_object()
    {
        ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);
        
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_component>();
        ces_entity::add_component(geometry_component);
    }
    
    renderable_game_object::~renderable_game_object()
    {
        
    }
    
    void renderable_game_object::add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material)
    {
        unsafe_get_material_component_from_this->add_material(technique_name, technique_pass, material);
    }
    
    void renderable_game_object::remove_material(const std::string& technique_name, i32 technique_pass)
    {
        unsafe_get_material_component_from_this->remove_material(technique_name, technique_pass);
    }
    
    material_shared_ptr renderable_game_object::get_material(const std::string& technique_name, i32 technique_pass) const
    {
        return unsafe_get_material_component_from_this->get_material(technique_name, technique_pass);
    }
    
    void renderable_game_object::set_visible(bool value)
    {
        unsafe_get_material_component_from_this->set_visible(value);
    }
    
    bool renderable_game_object::get_visible() const
    {
        return unsafe_get_material_component_from_this->get_visible();
    }
    
    void renderable_game_object::set_size(const glm::vec2& size)
    {
        unsafe_get_geometry_component_from_this->set_size(size);
    }
    
    glm::vec2 renderable_game_object::get_size() const
    {
        return unsafe_get_geometry_component_from_this->get_size();
    }
    
    void renderable_game_object::set_pivot(const glm::vec2& pivot)
    {
        unsafe_get_geometry_component_from_this->set_pivot(pivot);
    }
    
    glm::vec2 renderable_game_object::get_pivot() const
    {
        return unsafe_get_geometry_component_from_this->get_pivot();
    }
}