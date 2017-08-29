//
//  heightmap_textures_generator.h
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class heightmap_textures_generator
    {
    private:
        
        static void create_splatting_texture(const resource_accessor_shared_ptr& resource_accessor,
                                             const ces_entity_shared_ptr& entity,
                                             i32 i, i32 j, const std::function<void(i32 current, i32 total)>& callback);
        
        static void create_splatting_mask_textures(const ces_entity_shared_ptr& entity);
        
    protected:
        
    public:
        
        heightmap_textures_generator() = default;
        ~heightmap_textures_generator() = default;
        
        static void generate_splatting_mask_textures(const ces_entity_shared_ptr& entity);
        static void generate_splatting_diffuse_textures(const ces_entity_shared_ptr& entity, const resource_accessor_shared_ptr& resource_accessor, const std::function<void(i32 current, i32 total)>& callback);
    };
};


