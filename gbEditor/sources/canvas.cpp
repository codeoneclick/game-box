//
//  canvas.cpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "canvas.h"
#include "canvas_chunk.h"
#include "ces_material_component.h"
#include "ces_material_extension.h"

namespace gb
{
    namespace ed
    {
        canvas::canvas()
        {
            ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
            ces_entity::add_component(material_component);
            
            size.setter([=](const glm::vec2& size) {
                assert(static_cast<i32>(size.x) % static_cast<i32>(canvas_chunk::k_size) == 0);
                assert(static_cast<i32>(size.y) % static_cast<i32>(canvas_chunk::k_size) == 0);
                
                m_size = size / canvas_chunk::k_size;
                m_chunks.resize(m_size.x * m_size.y);
                
                for(i32 i = 0; i < m_size.x; ++i)
                {
                    for(i32 j = 0; j < m_size.y; ++j)
                    {
                        canvas_chunk_shared_ptr chunk = std::make_shared<canvas_chunk>();
                        for(const auto& iterator : m_materials)
                        {
                            for(const auto& material : iterator.second)
                            {
                                ces_material_extension::add_material(chunk, iterator.first, material.first,
                                                                     material.second);
                            }
                        }
                        m_chunks[i + j * m_size.x] = chunk;
                        canvas::add_child(chunk);
                    }
                }
                
            });
            size.getter([=]() {
                return glm::vec2(m_size.x * canvas_chunk::k_size,
                                 m_size.y * canvas_chunk::k_size);
            });
        }
        
        canvas::~canvas()
        {
            
        }
        
        void canvas::add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material)
        {
            m_materials[technique_name][technique_pass] = material;
        }
        
        void canvas::remove_material(const std::string& technique_name, i32 technique_pass)
        {
            const auto& iterator_01 = m_materials.find(technique_name);
            if(iterator_01 != m_materials.end())
            {
                const auto& iterator_02 = iterator_01->second.find(technique_pass);
                if(iterator_02 != iterator_01->second.end())
                {
                    m_materials[technique_name].erase(iterator_02);
                }
            }
        }
    }
}