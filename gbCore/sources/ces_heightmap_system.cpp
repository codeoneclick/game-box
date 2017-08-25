//
//  ces_heightmap_system.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_system.h"
#include "ces_heightmap_container_component.h"
#include "heightmap_mmap.h"
#include "heightmap_constants.h"
#include "vbo.h"

namespace gb
{
    ces_heightmap_system::ces_heightmap_system()
    {
        ces_base_system::add_required_component_guid(m_heightmap_components_mask, ces_heightmap_container_component::class_guid());
        ces_base_system::add_required_components_mask(m_heightmap_components_mask);
    }
    
    ces_heightmap_system::~ces_heightmap_system()
    {
        
    }
    
    void ces_heightmap_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_heightmap_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_heightmap_components_mask, [dt](const ces_entity_shared_ptr& entity) {

        });
    }
    
    void ces_heightmap_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_heightmap_system::create_vertices_data(const ces_entity_shared_ptr& entity)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_mmap = heightmap_container_component->get_mmap();
        
        heightmap_mmap::uncomressed_vertex *uncompressed_vertices = heightmap_mmap->get_uncopressed_vertices();
        heightmap_mmap::compressed_vertex *compressed_vertices = heightmap_mmap->get_compressed_vertices();
        heightmap_mmap::face *faces = heightmap_mmap->get_faces();
        glm::ivec2 heightmap_size = heightmap_mmap->get_heightmap_size();
        const std::vector<f32>& heights = heightmap_mmap->get_heights();
        
        for(ui32 i = 0; i < heightmap_size.x; ++i)
        {
            for(ui32 j = 0; j < heightmap_size.y; ++j)
            {
                uncompressed_vertices[i + j * heightmap_size.x].m_position = glm::vec3(i, heights[i + j * heightmap_size.x], j);
                uncompressed_vertices[i + j * heightmap_size.x].m_texcoord = glm::vec2(static_cast<f32>(i) / static_cast<f32>(heightmap_size.x),
                                                                                       static_cast<f32>(j) / static_cast<f32>(heightmap_size.y));
            }
        }
        
        ui32 index = 0;
        for(ui32 i = 0; i < (heightmap_size.x - 1); ++i)
        {
            for(ui32 j = 0; j < (heightmap_size.y - 1); ++j)
            {
                faces[index].m_indexes[0] = i + j * heightmap_size.x;
                uncompressed_vertices[faces[index].m_indexes[0]].m_contains_in_face[uncompressed_vertices[faces[index].m_indexes[0]].m_contains_in_face_size++] = index;
                glm::vec3 point_01 = uncompressed_vertices[faces[index].m_indexes[0]].m_position;
                faces[index].m_indexes[1] = i + (j + 1) * heightmap_size.x;
                uncompressed_vertices[faces[index].m_indexes[1]].m_contains_in_face[uncompressed_vertices[faces[index].m_indexes[1]].m_contains_in_face_size++] = index;
                glm::vec3 point_02 = uncompressed_vertices[faces[index].m_indexes[1]].m_position;
                faces[index].m_indexes[2] = i + 1 + j * heightmap_size.x;
                uncompressed_vertices[faces[index].m_indexes[2]].m_contains_in_face[uncompressed_vertices[faces[index].m_indexes[2]].m_contains_in_face_size++] = index;
                glm::vec3 point_03 = uncompressed_vertices[faces[index].m_indexes[2]].m_position;
                
                glm::vec3 edge_01 = point_02 - point_01;
                glm::vec3 edge_02 = point_03 - point_01;
                glm::vec3 normal = glm::cross(edge_01, edge_02);
                f32 sin = glm::length(normal) / (glm::length(edge_01) * glm::length(edge_02));
                faces[index].m_normal = glm::normalize(normal) * asinf(sin);
                index++;
                
                faces[index].m_indexes[0] = i + (j + 1) * heightmap_size.x;
                uncompressed_vertices[faces[index].m_indexes[0]].m_contains_in_face[uncompressed_vertices[faces[index].m_indexes[0]].m_contains_in_face_size++] = index;
                point_01 = uncompressed_vertices[faces[index].m_indexes[0]].m_position;
                faces[index].m_indexes[1] = i + 1 + (j + 1) * heightmap_size.x;
                uncompressed_vertices[faces[index].m_indexes[1]].m_contains_in_face[uncompressed_vertices[faces[index].m_indexes[1]].m_contains_in_face_size++] = index;
                point_02 = uncompressed_vertices[faces[index].m_indexes[1]].m_position;
                faces[index].m_indexes[2] = i + 1 + j * heightmap_size.x;
                uncompressed_vertices[faces[index].m_indexes[2]].m_contains_in_face[uncompressed_vertices[faces[index].m_indexes[2]].m_contains_in_face_size++] = index;
                point_03 = uncompressed_vertices[faces[index].m_indexes[2]].m_position;
                
                edge_01 = point_02 - point_01;
                edge_02 = point_03 - point_01;
                normal = glm::cross(edge_01, edge_02);
                sin = glm::length(normal) / (glm::length(edge_01) * glm::length(edge_02));
                faces[index].m_normal = glm::normalize(normal) * asinf(sin);
                index++;
            }
        }
        
        for(ui32 i = 0; i < heightmap_size.x * heightmap_size.y; ++i)
        {
            assert(uncompressed_vertices[i].m_contains_in_face_size != 0 && uncompressed_vertices[i].m_contains_in_face_size <= heightmap_constants::k_max_vertices_contains_in_face);
            glm::vec3 normal = faces[uncompressed_vertices[i].m_contains_in_face[0]].m_normal;
            for(ui32 j = 1; j < uncompressed_vertices[i].m_contains_in_face_size; ++j)
            {
                normal += faces[uncompressed_vertices[i].m_contains_in_face[j]].m_normal;
            }
            normal = glm::normalize(normal);
            uncompressed_vertices[i].m_normal = normal;
        }
        
        for(ui32 i = 0; i < heightmap_size.x * heightmap_size.y; ++i)
        {
            compressed_vertices[i].m_position = uncompressed_vertices[i].m_position;
            compressed_vertices[i].m_texcoord = glm::packUnorm2x16(uncompressed_vertices[i].m_texcoord);
            compressed_vertices[i].m_normal = glm::packSnorm4x8(glm::vec4(uncompressed_vertices[i].m_normal, 0.0f));
        }
        
        { // writing compressed vertices metadata
            std::ofstream stream;
            stream.open(heightmap_mmap::get_compressed_vertices_mmap_filename(heightmap_mmap->get_filename()), std::ios::binary | std::ios::out | std::ios::trunc);
            if(!stream.is_open())
            {
                assert(false);
            }
            
            for(ui32 i = 0; i < heightmap_size.x * heightmap_size.y; ++i)
            {
                stream.write((char*)&compressed_vertices[i], sizeof(heightmap_mmap::compressed_vertex));
            }
            stream.close();
        }
        
        { // writing uncompressed vertices metadata
            std::ofstream stream;
            stream.open(heightmap_mmap::get_uncompressed_vertices_mmap_filename(heightmap_mmap->get_filename()), std::ios::binary | std::ios::out | std::ios::trunc);
            if(!stream.is_open())
            {
                assert(false);
            }
            
            for(ui32 i = 0; i < heightmap_size.x * heightmap_size.y; ++i)
            {
                stream.write((char*)&uncompressed_vertices[i], sizeof(heightmap_mmap::uncomressed_vertex));
            }
            stream.close();
        }
        
        { // writing faces metadata
            std::ofstream stream;
            stream.open(heightmap_mmap::get_faces_mmap_filename(heightmap_mmap->get_filename()), std::ios::binary | std::ios::out | std::ios::trunc);
            if(!stream.is_open())
            {
                assert(false);
            }
            
            for(ui32 i = 0; i < (heightmap_size.x - 1) * (heightmap_size.y - 1) * 2; ++i)
            {
                stream.write((char*)&faces[i], sizeof(heightmap_mmap::face));
            }
            stream.close();
        }
    }
    
    void ces_heightmap_system::create_vbos_data(const ces_entity_shared_ptr& entity)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_mmap = heightmap_container_component->get_mmap();
        
        std::ofstream stream;
        stream.open(heightmap_mmap::get_vbos_mmap_filename(heightmap_mmap->get_filename()), std::ios::binary | std::ios::out | std::ios::trunc);
        if(!stream.is_open())
        {
            assert(false);
        }
        
        glm::ivec2 chunk_size = heightmap_container_component->get_chunk_size();
        glm::ivec2 chunks_count = heightmap_container_component->get_chunks_count();
        glm::ivec2 vertices_offset(0);
        vbo::vertex_attribute_PTNTCE vertex;
        for(ui32 i = 0; i < chunks_count.x; ++i)
        {
            vertices_offset.y = 0;
            for(ui32 j = 0; j < chunks_count.y; ++j)
            {
                for(ui32 x = 0; x < chunk_size.x; ++x)
                {
                    for(ui32 y = 0; y < chunk_size.y; ++y)
                    {
                        vertex.m_position = heightmap_mmap->get_vertex_position(x + vertices_offset.x, y + vertices_offset.y);
                        vertex.m_normal = heightmap_mmap->get_compressed_vertex_normal(x + vertices_offset.x, y + vertices_offset.y);
                        vertex.m_texcoord = glm::packUnorm2x16(glm::vec2(static_cast<f32>(x) / static_cast<f32>(chunk_size.x),
                                                                         static_cast<f32>(y) / static_cast<f32>(chunk_size.y)));
                        stream.write((char*)&vertex, sizeof(vbo::vertex_attribute_PTNTCE));
                    }
                }
                vertices_offset.y += chunk_size.y - 1;
            }
            vertices_offset.x += chunk_size.x - 1;
        }
        stream.close();
    }
    
    void ces_heightmap_system::create_ibos_data(const ces_entity_shared_ptr& entity)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_mmap = heightmap_container_component->get_mmap();
        
        std::ofstream stream;
        stream.open(heightmap_mmap::get_ibos_mmap_filename(heightmap_mmap->get_filename()), std::ios::binary | std::ios::out | std::ios::trunc);
        if(!stream.is_open())
        {
            assert(false);
        }
        
        glm::ivec2 chunk_size = heightmap_container_component->get_chunk_size();
        glm::ivec2 chunks_count = heightmap_container_component->get_chunks_count();
        
        for(ui32 i = 0; i < chunks_count.x; ++i)
        {
            for(ui32 j = 0; j < chunks_count.y; ++j)
            {
                for(ui32 k = 0; k < heightmap_constants::e_heightmap_lod_max; ++k)
                {
                    glm::ivec2 current_chunk_size = glm::ivec2(heightmap_container_component->get_chunk_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).x % 2 == 0 ?
                                                               heightmap_container_component->get_chunk_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).x :
                                                               heightmap_container_component->get_chunk_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).x - 1,
                                                               
                                                               heightmap_container_component->get_chunk_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).y % 2 == 0 ?
                                                               heightmap_container_component->get_chunk_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).y :
                                                               heightmap_container_component->get_chunk_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).y - 1);
                    
                    glm::ivec2 vertices_lod_offset = glm::ivec2(std::max((chunk_size.x - 1) / current_chunk_size.x, 1),
                                                                std::max((chunk_size.y - 1) / current_chunk_size.y, 1));
                    
                    i32 vertices_line_offset = chunk_size.x;
                    glm::ivec2 current_chunk_lod_start_index = glm::ivec2(0);
                    current_chunk_lod_start_index.x += k != heightmap_constants::e_heightmap_lod_01 ? 1 : 0;
                    current_chunk_lod_start_index.y += k != heightmap_constants::e_heightmap_lod_01 ? 1 : 0;
                    
                    std::vector<ui16> addition_indices;
                    if(k != heightmap_constants::e_heightmap_lod_01)
                    {
                        std::vector<ui16> current_lod_edge_indices;
                        for(ui32 x = 0; x <= current_chunk_size.x; ++x)
                        {
                            ui32 index = x * vertices_lod_offset.x + vertices_line_offset * vertices_lod_offset.y;
                            current_lod_edge_indices.push_back(index);
                        }
                        
                        ui32 current_lod_index = 0;
                        for(ui32 x = 0; x < chunk_size.x; ++x)
                        {
                            ui32 main_lod_index = x;
                            if(current_lod_edge_indices.size() > current_lod_index + 1)
                            {
                                if(x != 0 && x % vertices_lod_offset.x == 0)
                                {
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index]);
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index + 1]);
                                    addition_indices.push_back(main_lod_index);
                                    current_lod_index++;
                                }
                                
                                if(current_lod_edge_indices.size() - 1 == current_lod_index)
                                {
                                    break;
                                }
                                else
                                {
                                    addition_indices.push_back(main_lod_index);
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index]);
                                    addition_indices.push_back(main_lod_index + 1);
                                }
                            }
                        }
                        current_lod_edge_indices.clear();
                        
                        for(ui32 x = 0; x <= current_chunk_size.x; ++x)
                        {
                            ui32 index = x * vertices_lod_offset.x + vertices_line_offset * vertices_lod_offset.y * (current_chunk_size.y - 1);
                            current_lod_edge_indices.push_back(index);
                        }
                        
                        current_lod_index = 0;
                        for(ui32 x = 0; x < chunk_size.x; ++x)
                        {
                            ui32 main_lod_index = x + vertices_line_offset * (chunk_size.x - 1);
                            if(current_lod_edge_indices.size() > current_lod_index + 1)
                            {
                                if(x != 0 && x % vertices_lod_offset.x == 0)
                                {
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index]);
                                    addition_indices.push_back(main_lod_index);
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index + 1]);
                                    current_lod_index++;
                                }
                                
                                if(current_lod_edge_indices.size() - 1 == current_lod_index)
                                {
                                    break;
                                }
                                else
                                {
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index]);
                                    addition_indices.push_back(main_lod_index);
                                    addition_indices.push_back(main_lod_index + 1);
                                }
                            }
                        }
                        current_lod_edge_indices.clear();
                        
                        for(ui32 y = 0; y <= current_chunk_size.y; ++y)
                        {
                            ui32 index = vertices_lod_offset.x + y * vertices_line_offset * vertices_lod_offset.y;
                            current_lod_edge_indices.push_back(index);
                        }
                        
                        current_lod_index = 0;
                        for(ui32 y = 0; y < chunk_size.y; ++y)
                        {
                            ui32 main_lod_index = y * chunk_size.y;
                            if(current_lod_edge_indices.size() > current_lod_index + 1)
                            {
                                if(y != 0 && y % vertices_lod_offset.y == 0)
                                {
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index]);
                                    addition_indices.push_back(main_lod_index);
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index + 1]);
                                    current_lod_index++;
                                }
                                
                                if(current_lod_edge_indices.size() - 1 == current_lod_index)
                                {
                                    break;
                                }
                                else
                                {
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index]);
                                    addition_indices.push_back(main_lod_index);
                                    addition_indices.push_back(main_lod_index + chunk_size.y);
                                }
                            }
                        }
                        current_lod_edge_indices.clear();
                        
                        for(ui32 y = 0; y <= current_chunk_size.y; ++y)
                        {
                            ui32 index = vertices_lod_offset.x * (current_chunk_size.x - 1) + y * vertices_line_offset * vertices_lod_offset.y;
                            current_lod_edge_indices.push_back(index);
                        }
                        
                        current_lod_index = 0;
                        for(ui32 y = 0; y < chunk_size.y; ++y)
                        {
                            ui32 main_lod_index = (chunk_size.x - 1) + y * chunk_size.y;
                            if(current_lod_edge_indices.size() > current_lod_index + 1)
                            {
                                if(y != 0 && y % vertices_lod_offset.y == 0)
                                {
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index]);
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index + 1]);
                                    addition_indices.push_back(main_lod_index);
                                    current_lod_index++;
                                }
                                
                                if(current_lod_edge_indices.size() - 1 == current_lod_index)
                                {
                                    break;
                                }
                                else
                                {
                                    addition_indices.push_back(main_lod_index);
                                    addition_indices.push_back(current_lod_edge_indices[current_lod_index]);
                                    addition_indices.push_back(main_lod_index + chunk_size.y);
                                }
                            }
                        }
                        current_lod_edge_indices.clear();
                    }
                    
                    current_chunk_size.x -= k != heightmap_constants::e_heightmap_lod_01 ? 2 : 0;
                    current_chunk_size.y -= k != heightmap_constants::e_heightmap_lod_01 ? 2 : 0;
                    
                    ui32 indices_count = current_chunk_size.x * current_chunk_size.y * 6 + static_cast<ui32>(addition_indices.size());
                    
                    current_chunk_size.x += k != heightmap_constants::e_heightmap_lod_01 ? 1 : 0;
                    current_chunk_size.y += k != heightmap_constants::e_heightmap_lod_01 ? 1 : 0;
                    
                    ui16* indices = new ui16[indices_count];
                    
                    ui32 index = 0;
                    
                    for(ui32 x = 0; x < addition_indices.size(); ++x)
                    {
                        indices[index] = addition_indices[x];
                        index++;
                    }
                    
                    for(ui32 x = current_chunk_lod_start_index.x; x < current_chunk_size.x; ++x)
                    {
                        for(ui32 y = current_chunk_lod_start_index.y; y < current_chunk_size.y; ++y)
                        {
                            indices[index] = x * vertices_lod_offset.x + y * vertices_lod_offset.y * vertices_line_offset;
                            index++;
                            indices[index] = x * vertices_lod_offset.x + (y * vertices_lod_offset.y + vertices_lod_offset.y) * vertices_line_offset;
                            index++;
                            indices[index] = x * vertices_lod_offset.x + vertices_lod_offset.x + y * vertices_lod_offset.y * vertices_line_offset;
                            index++;
                            
                            indices[index] = x * vertices_lod_offset.x + (y * vertices_lod_offset.y + vertices_lod_offset.y) * vertices_line_offset;
                            index++;
                            indices[index] = x * vertices_lod_offset.x + vertices_lod_offset.x + (y * vertices_lod_offset.y + vertices_lod_offset.y) * vertices_line_offset;
                            index++;
                            indices[index] = x * vertices_lod_offset.x + vertices_lod_offset.x + y * vertices_lod_offset.y * vertices_line_offset;
                            index++;
                        }
                    }
                    
                    for(ui32 index_01 = 0; index_01 < 2; ++index_01)
                    {
                        for(ui32 index_02 = 0; index_02 < indices_count; ++index_02)
                        {
                            stream.write((char*)&indices[index_02], sizeof(ui16));
                        }
                    }
                    delete [] indices;
                }
            }
        }
        stream.close();
    }
    
    void ces_heightmap_system::create_tangent_space(const ces_entity_shared_ptr& entity)
    {
        
    }
    
    void ces_heightmap_system::create_attaches_to_vbo(const ces_entity_shared_ptr& entity)
    {
        
    }
    
    void ces_heightmap_system::generate_geometry(const ces_entity_shared_ptr& entity)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_mmap = heightmap_container_component->get_mmap();
        
        if(!heightmap_mmap::is_uncompressed_vertices_mmap_exist(heightmap_mmap->get_filename()) ||
           !heightmap_mmap::is_compressed_vertices_mmap_exist(heightmap_mmap->get_filename()) ||
           !heightmap_mmap::is_faces_mmap_exist(heightmap_mmap->get_filename()))
        {
            ces_heightmap_system::create_vertices_data(entity);
            ces_heightmap_system::create_vbos_data(entity);
            ces_heightmap_system::create_ibos_data(entity);
        }
        else
        {
            if(!heightmap_mmap::is_vbos_mmap_exist(heightmap_mmap->get_filename()))
            {
                ces_heightmap_system::create_vbos_data(entity);
            }
            
            if(!heightmap_mmap::is_ibos_mmap_exist(heightmap_mmap->get_filename()))
            {
                ces_heightmap_system::create_ibos_data(entity);
            }
        }
    }
    
    void ces_heightmap_system::generate_tangent_space(const ces_entity_shared_ptr& entity)
    {
        
    }
    
    void ces_heightmap_system::generate_attaches_to_vbo(const ces_entity_shared_ptr& entity)
    {
        
    }
}
