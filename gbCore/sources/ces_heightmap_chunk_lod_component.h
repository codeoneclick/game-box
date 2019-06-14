//
//  ces_heightmap_chunk_lod_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/30/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "heightmap_constants.h"

namespace gb
{
    class ces_heightmap_chunk_lod_component : public ces_base_component
    {
    private:
        
        heightmap_constants::e_heightmap_lod m_current_lod;
        heightmap_constants::e_heightmap_lod m_inprogress_lod;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_heightmap_chunk_lod_component, ces_base_component::g_guids_container)
        ces_heightmap_chunk_lod_component();
        ~ces_heightmap_chunk_lod_component();
        
        void set_inprogress_lod(heightmap_constants::e_heightmap_lod lod);
        heightmap_constants::e_heightmap_lod get_inprogress_lod() const;
        
        void set_current_lod(heightmap_constants::e_heightmap_lod lod);
        heightmap_constants::e_heightmap_lod get_current_lod() const;
    };
};

