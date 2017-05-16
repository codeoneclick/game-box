//
//  mesh.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"
#include "vbo.h"
#include "ibo.h"
#include "vao.h"
#include "shader.h"
#include "glm_extensions.h"

namespace gb
{
    class mesh_2d : public resource
    {
    private:
        
    protected:
        
        vbo_shared_ptr m_vbo;
        ibo_shared_ptr m_ibo;
        
        std::unordered_map<std::string, std::shared_ptr<vao>> m_vao_states;
        
        ui32 m_mode;
        
        virtual void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        virtual void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        mesh_2d(e_resource_type type, const std::string& guid);
        
#if !defined(__NO_RENDER__)

        mesh_2d(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, GLenum mode = GL_TRIANGLES);

#else

		mesh_2d(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, ui32 mode = 0);

#endif
        ~mesh_2d();
        
        ui32 get_id() const;
        
        vbo_shared_ptr get_vbo() const;
        ibo_shared_ptr get_ibo() const;
        
        virtual void bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
        virtual void draw() const;
        virtual void draw(ui32 indices) const;
        virtual void unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
        
        static bool intersect(const vbo_shared_ptr& vbo_01, const ibo_shared_ptr& ibo_01, const glm::mat4& mat_m_01, bool use_mat_m_01,
                              const vbo_shared_ptr& vbo_02, const ibo_shared_ptr& ibo_02, const glm::mat4& mat_m_02, bool use_mat_m_02,
                              std::vector<glm::triangle>* out_triangles_01 = nullptr, std::vector<glm::triangle>* out_triangles_02 = nullptr);
    };
};
