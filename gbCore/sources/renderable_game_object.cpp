//
//  renderable_game_object.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/10/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "renderable_game_object.h"
#include "ces_render_component.h"

namespace gb
{
    renderable_game_object::renderable_game_object()
    {
        ces_render_component_shared_ptr render_component = std::make_shared<ces_render_component>();
        ces_entity::add_component(render_component);
    }
    
    renderable_game_object::~renderable_game_object()
    {
        
    }
    
    void renderable_game_object::add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material)
    {
        unsafe_get_render_component_from_this->add_material(technique_name, technique_pass, material);
    }
    
    void renderable_game_object::remove_material(const std::string& technique_name, i32 technique_pass)
    {
        unsafe_get_render_component_from_this->remove_material(technique_name, technique_pass);
    }
    
    material_shared_ptr renderable_game_object::get_material(const std::string& technique_name, i32 technique_pass) const
    {
        return unsafe_get_render_component_from_this->get_material(technique_name, technique_pass);
    }
    
    void renderable_game_object::set_visible(bool value)
    {
        unsafe_get_render_component_from_this->set_visible(value);
    }
    
    bool renderable_game_object::get_visible() const
    {
        return unsafe_get_render_component_from_this->get_visible();
    }
}