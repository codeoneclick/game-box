//
//  heightmap_constants.h
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

namespace gb
{
    class heightmap_constants
    {
    public:
        
        enum e_heightmap_lod
        {
            e_heightmap_lod_unknown = -1,
            e_heightmap_lod_01 = 0,
            e_heightmap_lod_02,
            e_heightmap_lod_03,
            e_heightmap_lod_04,
            e_heightmap_lod_max
        };
        
        static const ui8 k_max_vertices_contains_in_face = 32;
        static const ui8 k_max_vertices_contains_in_vbo = 4;
        
        static const f32 k_min_splatting_texture_height;
        static const f32 k_max_splatting_texture_height;
        static const ui8 k_splatting_texture_channels;
        static const glm::ivec2 k_splatting_texture_mask_size;
        static const std::array<glm::ivec2, e_heightmap_lod_max> k_splatting_texture_size;
        
        static const f32 k_layer_section_01;
        static const f32 k_layer_section_02;
        static const f32 k_layer_section_offset;
        
        static const f32 k_raise;
        static const f32 k_deep;
    };
};
