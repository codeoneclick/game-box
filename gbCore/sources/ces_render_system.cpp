//
//  ces_render_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_render_system.h"
#include "render_pipeline.h"
#include "ces_system_types.h"
#include "ces_render_component.h"

namespace gb
{
    ces_render_system::ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen)
    {
        m_type = ces_system_type.ces_system_type_render;
        m_render_pipeline = std::make_shared<render_pipeline>(graphic_context, is_offscreen);
    }
    
    ces_render_system::~ces_render_system(void)
    {
        
    }
    
    std::shared_ptr<render_pipeline> ces_render_system::get_render_pipeline(void) const
    {
        return m_render_pipeline;
    }
    
    void ces_render_system::on_feed_start(f32 deltatime)
    {
        m_render_pipeline->on_draw_begin();
    }
    
    void ces_render_system::on_feed(const std::shared_ptr<ces_entity>& entity, f32 deltatime)
    {
        ces_render_component_shared_ptr render_component =
        std::static_pointer_cast<ces_render_component>(entity->get_component(e_ces_component_type_render));
        if(render_component)
        {
            m_render_pipeline->on_draw(entity);
        }
    }
    
    void ces_render_system::on_feed_end(f32 deltatime)
    {
        m_render_pipeline->on_draw_end();
    }
}