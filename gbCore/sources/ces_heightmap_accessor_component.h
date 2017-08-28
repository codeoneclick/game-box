//
//  ces_heightmap_accessor_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "heightmap_mmap.h"

namespace gb
{
    class ces_heightmap_accessor_component : public ces_base_component
    {
    private:
        
        std::shared_ptr<heightmap_mmap> m_heightmap_mmap;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_heightmap_accessor_component, ces_base_component::g_guids_container)
        ces_heightmap_accessor_component();
        ~ces_heightmap_accessor_component();
        
        void setup(const std::shared_ptr<heightmap_mmap>& heightmap_mmap);

        f32 get_height(const glm::vec3& position) const;
        glm::vec3 get_normal(const glm::vec3& position) const;
        glm::vec2 get_angles(const glm::vec3& position) const;
    };
};

