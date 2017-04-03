//
//  scene_fabricator.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class scene_fabricator
    {
    private:
        
    protected:
        
        configuration_accessor_shared_ptr m_configuration_accessor;
        resource_accessor_shared_ptr m_resource_accessor;
        
    public:
        
        scene_fabricator();
        ~scene_fabricator();
        
        void set_configuration_accessor(const configuration_accessor_shared_ptr& configuration_accessor);
        void set_resource_accessor(const resource_accessor_shared_ptr& resource_accessor);
        
        const configuration_accessor_shared_ptr get_configuration_accessor() const;
        const resource_accessor_shared_ptr get_resource_accessor() const;

#if !defined(__NO_RENDER__)

        void add_materials(const ces_entity_shared_ptr& entity,
                           const std::vector<std::shared_ptr<configuration>>& configurations);
        
        label_shared_ptr create_label(const std::string& filename);
        light_source_shared_ptr create_light_source(const std::string& filename);

#endif

		sprite_shared_ptr create_sprite(const std::string& filename);
    };
};
