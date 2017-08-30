//
//  ces_heightmap_chunk_lod_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/30/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_chunk_lod_component.h"

namespace gb
{
    ces_heightmap_chunk_lod_component::ces_heightmap_chunk_lod_component() :
    m_current_lod(heightmap_constants::e_heightmap_lod_unknown),
    m_inprogress_lod(heightmap_constants::e_heightmap_lod_unknown)
    {
        
    }
    
    ces_heightmap_chunk_lod_component::~ces_heightmap_chunk_lod_component()
    {
        
    }
    
    void ces_heightmap_chunk_lod_component::set_inprogress_lod(heightmap_constants::e_heightmap_lod lod)
    {
        m_inprogress_lod = lod;
    }
    
    heightmap_constants::e_heightmap_lod ces_heightmap_chunk_lod_component::get_inprogress_lod() const
    {
        return m_inprogress_lod;
    }
    
    void ces_heightmap_chunk_lod_component::set_current_lod(heightmap_constants::e_heightmap_lod lod)
    {
        m_current_lod = lod;
    }
    
    heightmap_constants::e_heightmap_lod ces_heightmap_chunk_lod_component::get_current_lod() const
    {
        return m_current_lod;
    }
}
