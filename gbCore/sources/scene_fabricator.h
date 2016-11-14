//
//  scene_fabricator.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef scene_fabricator_h
#define scene_fabricator_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class scene_fabricator
    {
    private:
        
    protected:
        
        std::set<game_object_shared_ptr> m_game_objects_container;
        configuration_accessor_shared_ptr m_configuration_accessor;
        resource_accessor_shared_ptr m_resource_accessor;
        
    public:
        
        scene_fabricator();
        ~scene_fabricator();
        
        void set_configuration_accessor(const configuration_accessor_shared_ptr& configuration_accessor);
        void set_resource_accessor(const resource_accessor_shared_ptr& resource_accessor);
        
        const configuration_accessor_shared_ptr get_configuration_accessor() const;
        const resource_accessor_shared_ptr get_resource_accessor() const;
        
        void add_materials(const ces_entity_shared_ptr& entity,
                           const std::vector<std::shared_ptr<configuration>>& configurations);
        
        sprite_shared_ptr create_sprite(const std::string& filename);
        text_label_shared_ptr create_text_label(const std::string& filename);
        light_source_shared_ptr create_light_source(const std::string& filename);
    };
};

#endif
