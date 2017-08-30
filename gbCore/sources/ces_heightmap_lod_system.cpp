//
//  ces_heightmap_lod_system.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/30/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_lod_system.h"
#include "ces_heightmap_container_component.h"
#include "ces_heightmap_preprocessing_component.h"
#include "ces_heightmap_chunks_component.h"
#include "ces_heightmap_bounding_boxes_component.h"
#include "ces_heightmap_chunk_lod_component.h"
#include "camera_3d.h"
#include "frustum_3d.h"
#include "game_object_3d.h"
#include "thread_operation.h"
#include "vbo.h"
#include "ibo.h"
#include "mesh_3d.h"

namespace gb
{
    ces_heightmap_lod_system::ces_heightmap_lod_system()
    {
        ces_base_system::add_required_component_guid(m_heightmap_components_mask, ces_heightmap_container_component::class_guid());
        ces_base_system::add_required_component_guid(m_heightmap_components_mask, ces_heightmap_preprocessing_component::class_guid());
        ces_base_system::add_required_component_guid(m_heightmap_components_mask, ces_heightmap_chunks_component::class_guid());
        ces_base_system::add_required_components_mask(m_heightmap_components_mask);
    }
    
    ces_heightmap_lod_system::~ces_heightmap_lod_system()
    {
        
    }
    
    void ces_heightmap_lod_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_heightmap_lod_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        auto camera = ces_base_system::get_current_camera_3d();
        auto frustum = camera->get_frustum();
        ces_base_system::enumerate_entities_with_components(m_heightmap_components_mask, [this, dt, camera, frustum](const ces_entity_shared_ptr& entity) {
            const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
            if(heightmap_container_component->get_is_generated())
            {
                const auto& heightmap_bounding_boxes_component = entity->get_component<ces_heightmap_bounding_boxes_component>();
                const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
                glm::ivec2 chunks_count = heightmap_container_component->get_chunks_count();
                auto& chunks = heightmap_chunks_component->get_chunks();
                
                for(i32 i = 0; i < chunks_count.x; ++i)
                {
                    for(i32 j = 0; j < chunks_count.y; ++j)
                    {
                        i32 index = i + j * chunks_count.x;
                        glm::vec3 min_bound = std::get<0>(heightmap_bounding_boxes_component->get_bounding_box(index));
                        glm::vec3 max_bound = std::get<1>(heightmap_bounding_boxes_component->get_bounding_box(index));
                        
                        i32 result = frustum->is_bound_box_in_frustum(max_bound, min_bound);
                        if((result == frustum_3d::e_frustum_bounds_result_inside ||
                            result == frustum_3d::e_frustum_bounds_result_intersect))
                        {
                            heightmap_constants::e_heightmap_lod lod = heightmap_chunks_component->get_chunk_lod(camera->get_look_at(), min_bound, max_bound);
                            if(chunks[index])
                            {
                                auto chunk = ces_entity::construct<game_object_3d>();
                                auto heightmap_chunk_lod_component = std::make_shared<ces_heightmap_chunk_lod_component>();
                                heightmap_chunk_lod_component->set_inprogress_lod(lod);
                                ces_heightmap_lod_system::load_heightmap_chunk(entity, i, j, lod, [this, entity, index, lod](const mesh_3d_shared_ptr& mesh){
                                    
                                }, [this, index, lod](const quad_tree_shared_ptr& quad_tree) {
                                    
                                }, [this, index, lod](const texture_shared_ptr& diffuse_texture, const texture_shared_ptr& normal_texture) {
                                    
                                });
                                /*m_heightmapAccessor->runLoading(i, j, LOD, [this, index, LOD](CSharedMeshRef mesh) {
                                 
                                 m_chunks[index]->onAddedToScene(m_renderTechniqueImporter,
                                 m_sceneUpdateMgr);
                                 m_chunks[index]->setMesh(mesh);
                                 m_chunks[index]->onConfigurationLoaded(m_configuration, true);
                                 
                                 }, [this, index, LOD](CSharedQuadTreeRef quadTree) {
                                 m_chunks[index]->setQuadTree(quadTree, LOD);
                                 }, [this, index](CSharedTextureRef DTexture, CSharedTextureRef NTexture) {
                                 m_chunks[index]->setPreprocessedSplattingDTexture(DTexture);
                                 m_chunks[index]->setPreprocessedSplattingNTexture(NTexture);
                                 });*/
                            }
                            else
                            {
                                auto chunk = chunks[index];
                                auto heightmap_chunk_lod_component = chunk->get_component<ces_heightmap_chunk_lod_component>();
                                
                                if(heightmap_chunk_lod_component->get_inprogress_lod() == heightmap_chunk_lod_component->get_current_lod() &&
                                   heightmap_chunk_lod_component->get_current_lod() != lod)
                                {
                                    heightmap_chunk_lod_component->set_inprogress_lod(lod);
                                    ces_heightmap_lod_system::load_heightmap_chunk(entity, i, j, lod, [this, entity, index, lod](const mesh_3d_shared_ptr& mesh){
                                        
                                    }, [this, index, lod](const quad_tree_shared_ptr& quad_tree) {
                                        
                                    }, [this, index, lod](const texture_shared_ptr& diffuse_texture, const texture_shared_ptr& normal_texture) {
                                        
                                    });
                                    /*m_chunks[index]->setInprogressLOD(LOD);
                                     m_heightmapAccessor->runLoading(i, j, LOD, [this, index, LOD](CSharedMeshRef mesh) {
                                     m_chunks[index]->setQuadTree(nullptr, m_chunks[index]->getCurrentLOD());
                                     m_chunks[index]->setMesh(mesh);
                                     m_chunks[index]->onSceneUpdate(0);
                                     }, [this, index, LOD](CSharedQuadTreeRef quadTree) {
                                     m_chunks[index]->setQuadTree(quadTree, LOD);
                                     m_chunks[index]->onSceneUpdate(0);
                                     }, [this, index](CSharedTextureRef DTexture, CSharedTextureRef NTexture) {
                                     m_chunks[index]->setPreprocessedSplattingDTexture(DTexture);
                                     m_chunks[index]->setPreprocessedSplattingNTexture(NTexture);
                                     });*/
                                }
                            }
                        }
                        else if(chunks[index])
                        {
                            auto chunk = chunks[index];
                            chunk->remove_from_parent();
                            ces_heightmap_lod_system::unload_heightmap_chunk(entity, i, j);
                            chunks[index] = nullptr;
                        }
                    }
                }
            }
        });
    }
    
    void ces_heightmap_lod_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_heightmap_lod_system::generate_mesh(const ces_entity_shared_ptr& entity, i32 index, heightmap_constants::e_heightmap_lod lod)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        
        std::shared_ptr<vbo::vertex_declaration_PTNTCE> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTNTCE>(4);
        vbo_shared_ptr vbo = nullptr;
        
#if !defined(__NO_RENDER__)
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, GL_STATIC_DRAW);
        
#else
        vbo = std::make_shared<gb::vbo>(vertex_declaration, 0);
        
#endif
        
        vbo::vertex_attribute_PTC *vertices = vbo->lock<vbo::vertex_attribute_PTC>();
        
        vertices[0].m_position = glm::vec3(-1.f, -1.f, 0.f);
        vertices[0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 0.f));
        vertices[1].m_position = glm::vec3(-1.f, 1.f, 0.f);
        vertices[1].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 1.f));
        vertices[2].m_position = glm::vec3(1.f, -1.f, 0.f);
        vertices[2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 0.f));
        vertices[3].m_position = glm::vec3(1.f, 1.f, 0.f);
        vertices[3].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 1.f));
        vbo->unlock();

        
        std::shared_ptr<CVertexBuffer> vbo = std::make_shared<CVertexBuffer>(m_container->getVBOMmap(index)->getSize(),
                                                                             GL_STATIC_DRAW,
                                                                             m_container->getVBOMmap(index)->getPointer());
        vbo->unlock();
        
        m_container->getIBOMmap(index, LOD)->updateSourcePointer();
        
        std::shared_ptr<CIndexBuffer> ibo = std::make_shared<CIndexBuffer>(m_container->getIBOMmap(index, LOD)->getSize(),
                                                                           GL_DYNAMIC_DRAW,
                                                                           m_container->getIBOMmap(index, LOD)->getSourcePointer());
        ibo->unlock();
        
        std::ostringstream stringstream;
        stringstream<<"chunk_"<<index<<"_"<<LOD<<std::endl;
        std::shared_ptr<CMesh> mesh = CMesh::construct(stringstream.str(), vbo, ibo,
                                                       std::get<0>(m_chunksBounds[index]), std::get<1>(m_chunksBounds[index]));
        std::get<0>(m_chunksMetadata[index]) = mesh;

    }
    
    void ces_heightmap_lod_system::generate_quad_tree(const ces_entity_shared_ptr& entity, i32 index)
    {
        
    }
    
    void ces_heightmap_lod_system::generate_splatting_textures(const ces_entity_shared_ptr& entity, i32 index, heightmap_constants::e_heightmap_lod lod)
    {
        
    }
    
    void ces_heightmap_lod_system::drop_metadata_cache(const ces_entity_shared_ptr& entity, i32 index)
    {
        const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
        auto& executed_operations = heightmap_chunks_component->get_executed_operations();
        assert(executed_operations[index] != nullptr);
        
        auto& callbacks = heightmap_chunks_component->get_callbacks();
        auto& chunks_metadata = heightmap_chunks_component->get_chunks_metadata();
        assert(std::get<5>(chunks_metadata[index]));
        std::lock_guard<std::mutex> guard(*std::get<5>(chunks_metadata[index]).get());
        
        auto& splatting_diffuse_textures_cache = heightmap_chunks_component->get_splatting_diffuse_textures_cache();
        if(splatting_diffuse_textures_cache[std::get<4>(chunks_metadata[index])].size() < heightmap_constants::k_splatting_textures_cache_size &&
           std::get<2>(chunks_metadata[index]))
        {
            splatting_diffuse_textures_cache[std::get<4>(chunks_metadata[index])].push(std::get<2>(chunks_metadata[index]));
        }
        
        /*if(m_splattingNTexturesCache[std::get<4>(m_chunksMetadata[index])].size() < kSplattingTexturesCacheSize &&
         std::get<3>(m_chunksMetadata[index]))
         {
         m_splattingNTexturesCache[std::get<4>(m_chunksMetadata[index])].push(std::get<3>(m_chunksMetadata[index]));
         }*/
        
        std::get<0>(chunks_metadata[index]) = nullptr;
        std::get<1>(chunks_metadata[index]) = nullptr;
        std::get<2>(chunks_metadata[index]) = nullptr;
        std::get<3>(chunks_metadata[index]) = nullptr;
        std::get<4>(chunks_metadata[index]) = heightmap_constants::e_heightmap_lod_unknown;
        
        std::get<0>(callbacks[index]) = nullptr;
        std::get<1>(callbacks[index]) = nullptr;
        std::get<2>(callbacks[index]) = nullptr;
        
        executed_operations[index] = nullptr;
    }
    
    void ces_heightmap_lod_system::load_heightmap_chunk(const ces_entity_shared_ptr& entity, i32 i, i32 j, heightmap_constants::e_heightmap_lod lod,
                                                        const ces_heightmap_chunks_component::heightmap_chunk_mesh_loaded_t heightmap_chunk_mesh_loaded,
                                                        const ces_heightmap_chunks_component::heightmap_chunk_quad_tree_loaded_t heightmap_chunk_quad_tree_loaded,
                                                        const ces_heightmap_chunks_component::heightmap_chunk_textures_loaded_t heightmap_chunk_textures_loaded)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
        
        glm::ivec2 chunks_count = heightmap_container_component->get_chunks_count();
        auto& executed_operations = heightmap_chunks_component->get_executed_operations();
        
        ui32 index = i + j * chunks_count.x;
        if(executed_operations[index] != nullptr)
        {
            return;
        }
        
        auto& callbacks = heightmap_chunks_component->get_callbacks();
        
        std::get<0>(callbacks[index]) = heightmap_chunk_mesh_loaded;
        std::get<1>(callbacks[index]) = heightmap_chunk_quad_tree_loaded;
        std::get<2>(callbacks[index]) = heightmap_chunk_textures_loaded;
        
        auto& chunks_metadata = heightmap_chunks_component->get_chunks_metadata();
        
        if(std::get<0>(chunks_metadata[index]) != nullptr)
        {
            std::get<0>(chunks_metadata[index]) = nullptr;
        }
        if(std::get<1>(chunks_metadata[index]) != nullptr)
        {
            std::get<1>(chunks_metadata[index]) = nullptr;
        }
        if(std::get<2>(chunks_metadata[index]) != nullptr)
        {
            std::get<2>(chunks_metadata[index]) = nullptr;
        }
        if(std::get<3>(chunks_metadata[index]) != nullptr)
        {
            std::get<3>(chunks_metadata[index]) = nullptr;
        }
        std::get<4>(chunks_metadata[index]) = lod;
        
        assert(std::get<0>(callbacks[index]) != nullptr);
        assert(std::get<1>(callbacks[index]) != nullptr);
        
        assert(std::get<0>(chunks_metadata[index]) == nullptr);
        assert(std::get<1>(chunks_metadata[index]) == nullptr);
        assert(std::get<2>(chunks_metadata[index]) == nullptr);
        assert(std::get<3>(chunks_metadata[index]) == nullptr);
        assert(std::get<4>(chunks_metadata[index]) != heightmap_constants::e_heightmap_lod_unknown);
        
        gb::thread_operation_shared_ptr completion_operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        completion_operation->set_execution_callback([entity, index]() {
            const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
            auto& callbacks = heightmap_chunks_component->get_callbacks();
            auto& chunks_metadata = heightmap_chunks_component->get_chunks_metadata();
            assert(std::get<1>(callbacks[index]) != nullptr);
            std::get<1>(callbacks[index])(std::get<1>(chunks_metadata[index]));
            auto& executed_operations = heightmap_chunks_component->get_executed_operations();
            executed_operations[index] = nullptr;
        });
        
        gb::thread_operation_shared_ptr generate_mesh_operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        generate_mesh_operation->set_execution_callback([this, entity, index, lod]() {
            const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
            auto& callbacks = heightmap_chunks_component->get_callbacks();
            auto& chunks_metadata = heightmap_chunks_component->get_chunks_metadata();
            ces_heightmap_lod_system::generate_mesh(entity, index, lod);
            assert(std::get<0>(callbacks[index]));
            std::get<0>(callbacks[index])(std::get<0>(chunks_metadata[index]));
        });
        completion_operation->add_dependency(generate_mesh_operation);
        
        gb::thread_operation_shared_ptr generate_splatting_textures_operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        generate_splatting_textures_operation->set_execution_callback([this, entity, index, lod](void) {
            const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
            auto& callbacks = heightmap_chunks_component->get_callbacks();
            auto& chunks_metadata = heightmap_chunks_component->get_chunks_metadata();
            ces_heightmap_lod_system::generate_splatting_textures(entity, index, lod);
            assert(std::get<2>(callbacks[index]));
            std::get<2>(callbacks[index])(std::get<2>(chunks_metadata[index]), std::get<3>(chunks_metadata[index]));
        });
        completion_operation->add_dependency(generate_splatting_textures_operation);
        
        gb::thread_operation_shared_ptr generate_quad_tree_operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_background);
        generate_quad_tree_operation->set_execution_callback([this, entity, index]() {
            ces_heightmap_lod_system::generate_quad_tree(entity, index);
        });
        completion_operation->add_dependency(generate_quad_tree_operation);
        
        assert(executed_operations[index] == nullptr);
        executed_operations[index] = completion_operation;
        
        completion_operation->set_cancel_callback([this, entity, index]() {
            ces_heightmap_lod_system::drop_metadata_cache(entity, index);
        });
        completion_operation->add_to_execution_queue();
    }
    
    void ces_heightmap_lod_system::unload_heightmap_chunk(const ces_entity_shared_ptr& entity, i32 i, i32 j)
    {
        
    }
}
