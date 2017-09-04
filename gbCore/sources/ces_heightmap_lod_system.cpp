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
#include "ces_heightmap_configuration_component.h"
#include "ces_material_component.h"
#include "ces_material_extension.h"
#include "ces_geometry_3d_component.h"
#include "camera_3d.h"
#include "frustum_3d.h"
#include "game_object_3d.h"
#include "thread_operation.h"
#include "configuration_accessor.h"
#include "vbo.h"
#include "ibo.h"
#include "mesh_3d.h"
#include "texture.h"

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
    
    void ces_heightmap_lod_system::set_resource_accessor(const resource_accessor_shared_ptr& resource_accessor)
    {
        m_resource_accessor = resource_accessor;
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
                            if(!chunks[index])
                            {
                                auto chunk = ces_entity::construct<game_object_3d>();
                                auto heightmap_chunk_lod_component = std::make_shared<ces_heightmap_chunk_lod_component>();
                                heightmap_chunk_lod_component->set_inprogress_lod(lod);
                                chunk->add_component(heightmap_chunk_lod_component);
                                auto material_component = std::make_shared<ces_material_component>();
                                chunk->add_component(material_component);
                                
                                auto heightmap_configuration_component = entity->get_component<ces_heightmap_configuration_component>();
                                
                                auto configuration_accessor = std::make_shared<gb::configuration_accessor>();
                                auto material_configuration = std::static_pointer_cast<gb::material_configuration>(configuration_accessor->get_material_configuration(heightmap_configuration_component->get_heightmap_chunk_material_filename()));
                                
                                auto material = gb::material::construct(std::static_pointer_cast<gb::material_configuration>(material_configuration));
                                gb::material::set_shader(material, std::static_pointer_cast<gb::material_configuration>(material_configuration), m_resource_accessor.lock(), true);
                                ces_material_extension::add_material(chunk, material_configuration->get_technique_name(),
                                                                     material_configuration->get_technique_pass(), material);
                                
                                ces_heightmap_lod_system::load_heightmap_chunk(entity, i, j, lod, [this, entity, index, lod](const mesh_3d_shared_ptr& mesh){
                                    const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
                                    auto& chunks = heightmap_chunks_component->get_chunks();
                                    auto chunk = chunks[index];
                                    auto geometry_3d_component = std::make_shared<ces_geometry_3d_component>();
                                    geometry_3d_component->set_mesh(mesh);
                                    chunk->add_component(geometry_3d_component);
                                }, [this, entity, index, lod](const quad_tree_shared_ptr& quad_tree) {
                                    
                                }, [this, entity, index, lod](const texture_shared_ptr& diffuse_texture, const texture_shared_ptr& normal_texture) {
                                    const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
                                    auto& chunks = heightmap_chunks_component->get_chunks();
                                    auto chunk = chunks[index];
                                    auto material_component = chunk->get_component<ces_material_component>();
                                    material_component->set_texture(diffuse_texture, gb::e_shader_sampler_01);
                                    entity->add_child(chunk);
                                });
                                chunks[index] = chunk;
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
                                        const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
                                        auto& chunks = heightmap_chunks_component->get_chunks();
                                        auto chunk = chunks[index];
                                        auto geometry_3d_component = chunk->get_component<ces_geometry_3d_component>();
                                        geometry_3d_component->set_mesh(mesh);

                                    }, [this, entity, index, lod](const quad_tree_shared_ptr& quad_tree) {
                                        
                                    }, [this, entity, index, lod](const texture_shared_ptr& diffuse_texture, const texture_shared_ptr& normal_texture) {
                                        const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
                                        auto& chunks = heightmap_chunks_component->get_chunks();
                                        auto chunk = chunks[index];
                                        auto material_component = chunk->get_component<ces_material_component>();
                                        material_component->set_texture(diffuse_texture, gb::e_shader_sampler_01);
                                    });
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
        const auto& heightmap_bounding_boxes_component = entity->get_component<ces_heightmap_bounding_boxes_component>();
        const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
        
        std::shared_ptr<vbo::vertex_declaration_PTNTCE> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTNTCE>(heightmap_container_component->get_vbo_mmap(index)->get_size(),
                                                                                                                              (vbo::vertex_attribute *)heightmap_container_component->get_vbo_mmap(index)->get_pointer());
        auto vbo = std::make_shared<gb::vbo>(vertex_declaration, GL_STATIC_DRAW);;
        vbo->unlock();
        
        heightmap_container_component->get_ibo_mmap(index, lod)->get_source_pointer();
        auto ibo = std::make_shared<gb::ibo>(heightmap_container_component->get_ibo_mmap(index, lod)->get_size(),
                                             GL_DYNAMIC_DRAW,
                                             false,
                                             heightmap_container_component->get_ibo_mmap(index, lod)->get_source_pointer());
        ibo->unlock();
        
        auto bounding_box = heightmap_bounding_boxes_component->get_bounding_box(index);
        std::ostringstream stringstream;
        stringstream<<"chunk_"<<index<<"_"<<lod<<std::endl;
        auto mesh = mesh_3d::construct(stringstream.str(), vbo, ibo,
                                       std::get<0>(bounding_box), std::get<1>(bounding_box));
        
        auto& chunks_metadata = heightmap_chunks_component->get_chunks_metadata();
        std::get<0>(chunks_metadata[index]) = mesh;
    }
    
    void ces_heightmap_lod_system::generate_quad_tree(const ces_entity_shared_ptr& entity, i32 index)
    {
        
    }
    
    void ces_heightmap_lod_system::generate_splatting_textures(const ces_entity_shared_ptr& entity, i32 index, heightmap_constants::e_heightmap_lod lod)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
        
        {
            auto& splatting_diffuse_textures_cache = heightmap_chunks_component->get_splatting_diffuse_textures_cache();
            texture_shared_ptr diffuse_texture = nullptr;
            if(!splatting_diffuse_textures_cache[lod].empty())
            {
                diffuse_texture = splatting_diffuse_textures_cache[lod].front();
                assert(diffuse_texture);
                splatting_diffuse_textures_cache[lod].pop();
            }
            else
            {
                std::ostringstream stringstream;
                stringstream<<"texture_"<<index<<std::endl;
                
                ui32 diffuse_texture_id;
                gl_create_textures(1, &diffuse_texture_id);
                
                diffuse_texture = texture::construct(stringstream.str(), diffuse_texture_id,
                                                     heightmap_container_component->get_textures_lod_size(lod).x,
                                                     heightmap_container_component->get_textures_lod_size(lod).y);
                
                diffuse_texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
                diffuse_texture->set_mag_filter(GL_LINEAR);
                diffuse_texture->set_min_filter(GL_LINEAR_MIPMAP_NEAREST);
            }
            
            diffuse_texture->bind();
            
            gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGB,
                               heightmap_container_component->get_textures_lod_size(lod).x, heightmap_container_component->get_textures_lod_size(lod).y,
                               0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, heightmap_container_component->get_splatting_diffuse_textures_mmap(index, lod)->get_pointer());
            gl_generate_mipmap(GL_TEXTURE_2D);
            
            auto& chunks_metadata = heightmap_chunks_component->get_chunks_metadata();
            std::get<2>(chunks_metadata[index]) = diffuse_texture;
        }
        /*
         {
         CSharedTexture NTexture = nullptr;
         if(!m_splattingNTexturesCache[LOD].empty())
         {
         NTexture = m_splattingNTexturesCache[LOD].front();
         assert(NTexture);
         m_splattingNTexturesCache[LOD].pop();
         }
         else
         {
         std::ostringstream stringstream;
         stringstream<<"NTexture_"<<index<<std::endl;
         
         ui32 NTextureId;
         ieGenTextures(1, &NTextureId);
         
         NTexture = CTexture::constructCustomTexture(stringstream.str(), NTextureId,
         m_container->getTexturesLODSize(LOD).x,
         m_container->getTexturesLODSize(LOD).y);
         NTexture->setWrapMode(GL_CLAMP_TO_EDGE);
         NTexture->setMagFilter(GL_LINEAR);
         NTexture->setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
         }
         
         NTexture->bind();
         
         ieTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
         m_container->getTexturesLODSize(LOD).x, m_container->getTexturesLODSize(LOD).y,
         0, GL_RGBA, GL_UNSIGNED_BYTE, m_container->getSplattingNTexturesMmap(index, LOD)->getPointer());
         ieGenerateMipmap(GL_TEXTURE_2D);
         
         std::get<3>(m_chunksMetadata[index]) = NTexture;
         }
         */
    }
    
    void ces_heightmap_lod_system::drop_metadata_cache(const ces_entity_shared_ptr& entity, i32 index)
    {
        const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
        auto& executed_operations = heightmap_chunks_component->get_executed_operations();
        
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
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_chunks_component = entity->get_component<ces_heightmap_chunks_component>();
        
        glm::ivec2 chunks_count = heightmap_container_component->get_chunks_count();
        auto& executed_operations = heightmap_chunks_component->get_executed_operations();
        
        ui32 index = i + j * chunks_count.x;
        
        if(executed_operations[index])
        {
            executed_operations[index]->cancel();
        }
        else
        {
            ces_heightmap_lod_system::drop_metadata_cache(entity, index);
        }
    }
}
