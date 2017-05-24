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
#include "mesh_2d.h"

namespace gb
{
    class ces_render_system : public ces_base_system
    {
    private:
        
        std::shared_ptr<render_pipeline> m_render_pipeline;
        batching_pipeline_shared_ptr m_batching_pipeline;
        std::shared_ptr<mesh_2d<vertex_attribute>> m_camera_2d_mesh;
        glm::vec4 m_camera_2d_bounds;
        
        std::map<std::string, std::map<i32, std::queue<ces_entity_weak_ptr>>> m_visible_entities;
        std::map<std::string, std::map<i32, std::queue<ces_entity_weak_ptr>>> m_visible_lights;
        
        void draw_entities(const std::string &technique_name, i32 technique_pass);
        void draw_lights(const std::string &technique_name, i32 technique_pass);
        
        void grab_visible_entities_recursively(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass);
        void grab_visible_lights_recursively(const ces_entity_shared_ptr& entity, const std::string &technique_name, i32 technique_pass);
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        CTTI_CLASS_GUID(ces_render_system, ces_base_system::g_guids_container)
        ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen);
        ~ces_render_system();
        
        std::shared_ptr<render_pipeline> get_render_pipeline() const;
    };
};

#endif
