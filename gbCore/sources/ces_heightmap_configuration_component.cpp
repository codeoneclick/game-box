//
//  ces_heightmap_configuration_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/29/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_configuration_component.h"

namespace gb
{
    ces_heightmap_configuration_component::ces_heightmap_configuration_component()
    {
        
    }
    
    ces_heightmap_configuration_component::~ces_heightmap_configuration_component()
    {
        
    }
    
    void ces_heightmap_configuration_component::setup(const std::string& splatting_preprocessing_material_filename, const std::string& heightmap_chunk_material_filename)
    {
        m_splatting_preprocessing_material_filename = splatting_preprocessing_material_filename;
        m_heightmap_chunk_material_filename = heightmap_chunk_material_filename;
    }
    
    std::string ces_heightmap_configuration_component::get_splatting_preprocessing_material_filename() const
    {
        return m_splatting_preprocessing_material_filename;
    }
    
    std::string ces_heightmap_configuration_component::get_heightmap_chunk_material_filename() const
    {
        return m_heightmap_chunk_material_filename;
    }
}
