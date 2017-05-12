//
//  mesh_3d_commiter_gbmesh3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "mesh_3d_commiter_gbmesh3d.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "ibo.h"

namespace gb
{
    mesh_3d_commiter_gbmesh3d::mesh_3d_commiter_gbmesh3d(const std::string& guid, const resource_shared_ptr& resource) :
    resource_commiter(guid, resource)
    {
        
    }
    
    mesh_3d_commiter_gbmesh3d::~mesh_3d_commiter_gbmesh3d()
    {
        
    }
    
    void mesh_3d_commiter_gbmesh3d::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        
        mesh_3d_shared_ptr mesh = std::static_pointer_cast<mesh_3d>(m_resource);
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(mesh->get_num_raw_vertices(), GL_STATIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();
        
        for(ui32 i = 0; i < mesh->get_num_raw_vertices(); ++i)
        {
            vertices[i].m_position = mesh->get_raw_vertices()[i].m_position;
            glm::vec2 texcoord = mesh->get_raw_vertices()[i].m_texcoord;
            vertices[i].m_texcoord = glm::packUnorm2x16(texcoord);
            glm::vec3 normal = mesh->get_raw_vertices()[i].m_normal;
            vertices[i].m_normal = glm::packSnorm4x8(glm::vec4(normal.x, normal.y, normal.z, 0.0));
            glm::vec3 tangent = mesh->get_raw_vertices()[i].m_tangent;
            vertices[i].m_tangent = glm::packSnorm4x8(glm::vec4(tangent.x, tangent.y, tangent.z, 0.0));
            
            assert(mesh->get_raw_vertices()[i].m_bones.size() <= 4);
            vertices[i].m_color = glm::u8vec4(mesh->get_raw_vertices()[i].m_bones.size() >= 1 ? static_cast<ui8>(mesh->get_raw_vertices()[i].m_bones[0].m_id) : 0,
                                              mesh->get_raw_vertices()[i].m_bones.size() >= 2 ? static_cast<ui8>(mesh->get_raw_vertices()[i].m_bones[1].m_id) : 0,
                                              mesh->get_raw_vertices()[i].m_bones.size() >= 3 ? static_cast<ui8>(mesh->get_raw_vertices()[i].m_bones[2].m_id) : 0,
                                              mesh->get_raw_vertices()[i].m_bones.size() == 4 ? static_cast<ui8>(mesh->get_raw_vertices()[i].m_bones[3].m_id) : 0);
            
            vertices[i].m_extra = glm::u8vec4(mesh->get_raw_vertices()[i].m_bones.size() >= 1 ? static_cast<ui8>(mesh->get_raw_vertices()[i].m_bones[0].m_weigth * 255.f) : 0,
                                              mesh->get_raw_vertices()[i].m_bones.size() >= 2 ? static_cast<ui8>(mesh->get_raw_vertices()[i].m_bones[1].m_weigth * 255.f) : 0,
                                              mesh->get_raw_vertices()[i].m_bones.size() >= 3 ? static_cast<ui8>(mesh->get_raw_vertices()[i].m_bones[2].m_weigth * 255.f) : 0,
                                              mesh->get_raw_vertices()[i].m_bones.size() == 4 ? static_cast<ui8>(mesh->get_raw_vertices()[i].m_bones[3].m_weigth * 255.f) : 0);
        }
        vbo->unlock();
        
        std::shared_ptr<ibo> ibo = std::make_shared<gb::ibo>(mesh->get_num_raw_indices(), GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        memcpy(indices, &mesh->get_raw_indices()[0], sizeof(ui16) * mesh->get_num_raw_indices());
        ibo->unlock();
        
        GLenum error = glGetError();
        assert(error == GL_NO_ERROR);
        
        m_status = e_commiter_status_success;
        resource_commiter::on_transfering_data_commited(vbo);
        resource_commiter::on_transfering_data_commited(ibo);
    }
}
