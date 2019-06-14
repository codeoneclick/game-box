//
//  ces_heightmap_configuration_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/29/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_heightmap_configuration_component : public ces_base_component
    {
    private:
        
        std::string m_splatting_preprocessing_material_filename;
        std::string m_heightmap_chunk_material_filename;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_heightmap_configuration_component, ces_base_component::g_guids_container)
        ces_heightmap_configuration_component();
        ~ces_heightmap_configuration_component();
        
        void setup(const std::string& splatting_preprocessing_material_filename, const std::string& heightmap_chunk_material_filename);
        std::string get_splatting_preprocessing_material_filename() const;
        std::string get_heightmap_chunk_material_filename() const;
    };
};
