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
}