//
//  ces_heightmap_lod_system.h
//  gbCore
//
//  Created by serhii serhiiv on 8/30/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"
#include "heightmap_constants.h"
#include "ces_heightmap_chunks_component.h"

namespace gb
{
    class ces_heightmap_lod_system : public ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_heightmap_components_mask;
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
        void drop_metadata_cache(const ces_entity_shared_ptr& entity, i32 index);
        
        void generate_mesh(const ces_entity_shared_ptr& entity, i32 index, heightmap_constants::e_heightmap_lod lod);
        void generate_quad_tree(const ces_entity_shared_ptr& entity, i32 index);
        void generate_splatting_textures(const ces_entity_shared_ptr& entity, i32 index, heightmap_constants::e_heightmap_lod lod);
        
        void load_heightmap_chunk(const ces_entity_shared_ptr& entity, i32 i, i32 j, heightmap_constants::e_heightmap_lod lod,
                                  const ces_heightmap_chunks_component::heightmap_chunk_mesh_loaded_t heightmap_chunk_mesh_loaded,
                                  const ces_heightmap_chunks_component::heightmap_chunk_quad_tree_loaded_t heightmap_chunk_quad_tree_loaded,
                                  const ces_heightmap_chunks_component::heightmap_chunk_textures_loaded_t heightmap_chunk_textures_loaded);
        void unload_heightmap_chunk(const ces_entity_shared_ptr& entity, i32 i, i32 j);
        
    public:
        
        CTTI_CLASS_GUID(ces_heightmap_lod_system, ces_base_system::g_guids_container)
        ces_heightmap_lod_system();
        ~ces_heightmap_lod_system();
    };
};
