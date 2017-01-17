//
//  ces_text_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

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
        auto text_component = entity->get_unsafe_component<ces_text_component>();
        if(text_component && text_component->is_text_changed())
        {
            auto geometry_component = entity->get_unsafe_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(text_component->generate_geometry());
            text_component->reset();
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_text_system::update_recursively(child);
        }
    }
}

#endif