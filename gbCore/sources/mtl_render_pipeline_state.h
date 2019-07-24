//
//  mtl_render_pipeline_state.hpp
//  gbCore
//
//  Created by serhii.s on 3/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == METAL_API

#include "material.h"

namespace gb
{
    class i_mtl_render_pipeline_state_impl : public std::enable_shared_from_this<i_mtl_render_pipeline_state_impl>
    {
    public:
        
        template <typename T>
        std::shared_ptr<T> as() const
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        virtual void* get_mtl_raw_render_pipeline_state_ptr() const = 0;
        
        virtual std::string get_guid() const = 0;
    };
    
    class mtl_render_pipeline_state
    {
    private:
        
        std::shared_ptr<i_mtl_render_pipeline_state_impl> m_mtl_render_pipeline_state_impl = nullptr;
        
        static std::unordered_map<std::string, std::shared_ptr<mtl_render_pipeline_state>> m_render_pipelines_pool;
        
    protected:
        
        template <typename T>
        std::shared_ptr<T> impl_as() const
        {
            return std::static_pointer_cast<T>(m_mtl_render_pipeline_state_impl);
        };
        
    public:
        
        mtl_render_pipeline_state(const std::shared_ptr<material_cached_parameters>& material_parameters,
                                  const mtl_vertex_descriptor_shared_ptr& vertex_descriptor);
        ~mtl_render_pipeline_state();
        
        std::shared_ptr<mtl_render_pipeline_state> static construct(const std::shared_ptr<material_cached_parameters>& material_parameters,
                                                                    const mtl_vertex_descriptor_shared_ptr& vertex_descriptor);
        
        void* get_mtl_raw_render_pipeline_state_ptr() const;
        
        std::string get_guid() const;
    };
};

#endif
