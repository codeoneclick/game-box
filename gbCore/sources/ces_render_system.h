//
//  ces_render_system.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

// #define __USE_BATCHING__ 1

namespace gb
{
    class ces_render_system : public ces_base_system
    {
    private:
        
        typedef std::function<void(const std::string&,
                                   i32,
                                   ces_material_component*,
                                   ces_geometry_component*,
                                   ces_transformation_component*,
                                   ces_shader_uniforms_component*)> draw_3d_entity_func_t;
        
        std::shared_ptr<render_pipeline> m_render_pipeline;
        
#if defined(__USE_BATCHING__)
        
        batching_pipeline_shared_ptr m_batching_pipeline;
        
#endif
        
#if USED_GRAPHICS_API != METAL_API
        
        ogl_graveyard_controller_shared_ptr m_ogl_graveyard_controller;
        
#endif
        
        mesh_2d_shared_ptr m_camera_2d_mesh;
        glm::vec4 m_camera_2d_bounds;
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_render_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_render_target_components_mask;
        
        std::vector<std::map<f32, ces_entity_weak_ptr>> m_visible_2d_entities;
        std::vector<std::queue<ces_entity_weak_ptr>> m_visible_3d_entities;
        
        void draw_2d_entities(const std::string &technique_name);
        void draw_3d_entities(const std::string &technique_name);
        
        void draw_3d_entity(const std::string& technique_name,
                            i32 technique_index,
                            ces_material_component* material_component,
                            ces_geometry_component* geometry_component,
                            ces_transformation_component* transformation_component,
                            ces_shader_uniforms_component* shader_uniforms_component);
        
        void grab_visible_entities(const std::string &technique_name, const draw_3d_entity_func_t& draw_3d_entity_func = nullptr);
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
    public:
        
        STTI_CLASS_GUID(ces_render_system, ces_base_system::g_guids_container)
        ces_render_system(const std::shared_ptr<graphics_context>& graphic_context, bool is_offscreen);
        ~ces_render_system();
        
        std::shared_ptr<render_pipeline> get_render_pipeline() const;
    };
};

#endif
