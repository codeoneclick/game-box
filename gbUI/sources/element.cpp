//
//  element.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "element.h"
#include "ces_transformation_component.h"
#include "ces_geometry_component.h"
#include "ces_render_component.h"

namespace gb
{
    namespace ui
    {
        element::element() :
        m_position(glm::ivec2(0)),
        m_size(glm::ivec2(2))
        {
            ces_transformation_component_shared_ptr transformation_component = std::make_shared<ces_transformation_component>();
            ces_entity::add_component(transformation_component);
            
            ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_component>();
            ces_entity::add_component(geometry_component);
            
            ces_render_component_shared_ptr render_component = std::make_shared<ces_render_component>();
            ces_entity::add_component(render_component);
        }
        
        element::~element()
        {
            
        }
        
        void element::set_position(const glm::ivec2& position)
        {
            m_position = position;
        }
        
        glm::ivec2 element::get_position() const
        {
            return m_position;
        }
        
        void element::set_size(const glm::ivec2& size)
        {
            m_size = size;
        }
        
        glm::ivec2 element::get_size() const
        {
            return m_size;
        }
        
        void element::add_material(const std::string& technique_name, const material_shared_ptr& material)
        {
            unsafe_get_render_component_from_this->add_material(technique_name, 0, material);
        }
        
        void element::remove_material(const std::string& technique_name)
        {
            unsafe_get_render_component_from_this->remove_material(technique_name , 0);
        }
        
        material_shared_ptr element::get_material(const std::string& technique_name) const
        {
            return unsafe_get_render_component_from_this->get_material(technique_name, 0);
        }
        
        void element::set_mesh(const mesh_shared_ptr& mesh)
        {
            unsafe_get_geometry_component_from_this->set_mesh(mesh);
        }
    }
}