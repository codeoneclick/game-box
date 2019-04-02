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

#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        bool is_using_batch(const std::vector<std::shared_ptr<configuration>>& configurations);

        void add_materials(const ces_entity_shared_ptr& entity,
                           const std::vector<std::shared_ptr<configuration>>& configurations);
        void add_animation_sequences_3d(const ces_entity_shared_ptr& entity,
                                        const mesh_3d_shared_ptr& mesh,
                                        const std::vector<std::shared_ptr<configuration>>& configurations);
        
#endif

		sprite_shared_ptr create_sprite(const std::string& filename);
        shape_3d_shared_ptr create_shape_3d(const std::string& filename);
        label_shared_ptr create_label(const std::string& filename);
        light_source_2d_shared_ptr create_light_source_2d(const std::string& filename);
        heightmap_shared_ptr create_heightmap(const std::string& filename);
        deferred_point_light_3d_shared_ptr create_deferred_point_light_3d(const std::string& filename);
        deferred_spot_light_3d_shared_ptr create_deferred_spot_light_3d(const std::string& filename);
        particle_emitter_shared_ptr create_particle_emitter(const std::string& filename);
        trail_shared_ptr create_trail(const std::string& filename);
    };
};
