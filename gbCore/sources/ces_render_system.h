//
//  ces_render_system.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "ces_base_system.h"

namespace gb
{
    class ces_render_system : public ces_base_system
    {
    private:
        
        std::shared_ptr<render_pipeline> m_render_pipeline;
        batching_pipeline_shared_ptr m_batching_pipeline;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void draw_recursively(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass);
        void draw_recursively_lights(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass);
        void draw_shadow(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass);
        
    public:
        
        CTTI_CLASS_GUID(ces_render_system, ces_base_system::g_guids_container)
        ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen);
        ~ces_render_system();
        
        std::shared_ptr<render_pipeline> get_render_pipeline() const;
    };
};

#endif
