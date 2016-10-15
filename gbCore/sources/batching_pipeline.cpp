//
//  batching_pipeline.cpp
//  gbCore
//
//  Created by serhii serhiiv on 10/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "batching_pipeline.h"
#include "batch.h"
#include "material.h"
#include "mesh.h"
#include "vbo.h"
#include "ibo.h"

namespace gb
{
    const ui32 batching_pipeline::k_max_num_batches_with_same_material = 8;
    
    batching_pipeline::batching_pipeline()
    {
        
    }
    
    batching_pipeline::~batching_pipeline()
    {
        
    }
    
    void batching_pipeline::batch(const material_shared_ptr& material, const mesh_shared_ptr& mesh, const glm::mat4& matrix)
    {
        assert(material != nullptr);
        assert(material->get_shader() != nullptr);
        assert(mesh->get_vbo()->get_used_size() <= batch::k_max_num_vertices);
        assert(mesh->get_ibo()->get_used_size() <= batch::k_max_num_indices);
        
        batch_shared_ptr batch = nullptr;
        for(ui32 i = 0; i < k_max_num_batches_with_same_material; ++i)
        {
            std::ostringstream stream;
            stream<<i;
            std::string guid = material->get_guid() + stream.str();
            auto iterator = m_batches.find(guid);
            
            if(iterator == m_batches.end())
            {
                batch = std::make_shared<gb::batch>(material);
                m_batches.insert(std::make_pair(guid, batch));
                break;
            }
            else if(iterator->second->get_num_vertices() + mesh->get_vbo()->get_used_size() > batch::k_max_num_vertices ||
                    iterator->second->get_num_indices() + mesh->get_ibo()->get_used_size() > batch::k_max_num_indices)
            {
                continue;
            }
            else
            {
                batch = iterator->second;
                break;
            }
            assert(false);
        }
        
        batch->add(mesh, matrix);
    }
    
    void batching_pipeline::draw()
    {
        for(const auto& iterator : m_batches)
        {
            iterator.second->draw();
            iterator.second->reset();
        }
    }
}
