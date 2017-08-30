//
//  ces_heightmap_preprocessing_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/29/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_preprocessing_component.h"

namespace gb
{
    ces_heightmap_preprocessing_component::ces_heightmap_preprocessing_component() :
    m_splatting_preprocessing_material(nullptr),
    m_splatting_mask_texture_buffer(nullptr),
    m_executed_preprocessing_operations_count(0),
    m_splatting_processing_stream(nullptr)
    {
        m_render_targets.fill(nullptr);
    }
    
    ces_heightmap_preprocessing_component::~ces_heightmap_preprocessing_component()
    {
        
    }

    void ces_heightmap_preprocessing_component::set_render_target(const render_target_shared_ptr& render_target, heightmap_constants::e_heightmap_lod lod)
    {
        m_render_targets.at(lod) = render_target;
    }
    
    render_target_shared_ptr ces_heightmap_preprocessing_component::get_render_traget(heightmap_constants::e_heightmap_lod lod) const
    {
        return m_render_targets.at(lod);
    }
    
    void ces_heightmap_preprocessing_component::set_splatting_preprocessing_material(const material_shared_ptr& material)
    {
        m_splatting_preprocessing_material = material;
    }
    
    material_shared_ptr ces_heightmap_preprocessing_component::get_splatting_preprocessing_material() const
    {
        return m_splatting_preprocessing_material;
    }
    
    void ces_heightmap_preprocessing_component::set_splatting_mask_texture_buffer(const texture_shared_ptr& texture)
    {
        m_splatting_mask_texture_buffer = texture;
    }
    
    texture_shared_ptr ces_heightmap_preprocessing_component::get_splatting_mask_texture_buffer() const
    {
        return m_splatting_mask_texture_buffer;
    }
    
    void ces_heightmap_preprocessing_component::set_splatting_processing_stream(const std::shared_ptr<std::ofstream>& stream)
    {
        m_splatting_processing_stream = stream;
    }
    
    std::shared_ptr<std::ofstream> ces_heightmap_preprocessing_component::get_splatting_processing_stream() const
    {
        return m_splatting_processing_stream;
    }
    
    void ces_heightmap_preprocessing_component::inc_executed_preprocessing_operations_count()
    {
        m_executed_preprocessing_operations_count++;
    }
    
    void ces_heightmap_preprocessing_component::dec_executed_preprocessing_operations_count()
    {
        m_executed_preprocessing_operations_count--;
    }
    
    i32 ces_heightmap_preprocessing_component::get_executed_preprocessing_operations_count()
    {
        return m_executed_preprocessing_operations_count;
    }
}
