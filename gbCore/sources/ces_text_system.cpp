//
//  ces_text_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_text_system.h"
#include "ces_text_component.h"
#include "ces_geometry_freeform_component.h"
#include "vbo.h"
#include "ibo.h"
#include "mesh.h"

namespace gb
{
    ces_text_system::ces_text_system()
    {
        m_type = e_ces_system_type_text;
    }
    
    ces_text_system::~ces_text_system()
    {
        
    }
    
    void ces_text_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_text_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_text_system::update_recursively(entity);
    }
    
    void ces_text_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_text_system::update_recursively(const ces_entity_shared_ptr& entity)
    {
        ces_text_component* text_component = unsafe_get_text_component(entity);
        if(text_component && text_component->is_text_changed())
        {
            ces_geometry_freeform_component* geometry_freeform_component = unsafe_get_geometry_freeform_component(entity);
            geometry_freeform_component->set_mesh(text_component->generate_geometry());
            text_component->reset();
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_text_system::update_recursively(child);
        }
    }
}
