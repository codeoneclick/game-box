//
//  ces_heightmap_preprocessing_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/29/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "heightmap_constants.h"

namespace gb
{
    class ces_heightmap_preprocessing_component : public ces_base_component
    {
    private:
        
        i32 m_executed_preprocessing_operations_count;
        std::array<render_target_shared_ptr, heightmap_constants::e_heightmap_lod_max> m_render_targets;
        material_shared_ptr m_splatting_preprocessing_material;
        texture_shared_ptr m_splatting_mask_texture_buffer;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_heightmap_preprocessing_component, ces_base_component::g_guids_container)
        ces_heightmap_preprocessing_component();
        ~ces_heightmap_preprocessing_component();
        
        void set_render_target(const render_target_shared_ptr& render_target, heightmap_constants::e_heightmap_lod lod);
        render_target_shared_ptr get_render_traget(heightmap_constants::e_heightmap_lod lod) const;
        
        void set_splatting_preprocessing_material(const material_shared_ptr& material);
        material_shared_ptr get_splatting_preprocessing_material() const;
        
        void set_splatting_mask_texture_buffer(const texture_shared_ptr& texture);
        texture_shared_ptr get_splatting_mask_texture_buffer() const;
        
        void inc_executed_preprocessing_operations_count();
        void dec_executed_preprocessing_operations_count();
        i32 get_executed_preprocessing_operations_count();
    };
};

