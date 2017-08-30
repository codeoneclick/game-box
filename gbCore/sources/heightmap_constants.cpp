//
//  heightmap_constants.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "heightmap_constants.h"

namespace gb
{
    const f32 heightmap_constants::k_min_splatting_texture_height = 32.f;
    const f32 heightmap_constants::k_max_splatting_texture_height = 32.f;
    const ui8 heightmap_constants::k_splatting_texture_channels = 4;
    const glm::ivec2 heightmap_constants::k_splatting_texture_mask_size = glm::ivec2(64);
    const std::array<glm::ivec2, heightmap_constants::e_heightmap_lod_max> heightmap_constants::k_splatting_texture_size = { glm::ivec2(1024), glm::ivec2(512), glm::ivec2(256), glm::ivec2(128) };
    
    const f32 heightmap_constants::k_raise = 32.f;
    const f32 heightmap_constants::k_deep = heightmap_constants::k_raise * .25f;
    
    const f32 heightmap_constants::k_layer_section_01 = .33f;
    const f32 heightmap_constants::k_layer_section_02 = .66f;
    const f32 heightmap_constants::k_layer_section_offset = .11f;
    
    const f32 heightmap_constants::k_lod_01_distance = 64.f;
    const f32 heightmap_constants::k_lod_02_distance = 192.f;
    const f32 heightmap_constants::k_lod_03_distance = 256.f;
    
    const i32 heightmap_constants::k_splatting_textures_cache_size = 4;
}
