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

namespace gb
{
    class mesh
    {
    private:
        
    protected:
        
        vbo_shared_ptr m_vbo;
        ibo_shared_ptr m_ibo;
        
        std::unordered_map<std::string, std::shared_ptr<vao>> m_vao_states;
        
        ui32 m_mode;
        
    public:
        
#if !defined(__NO_RENDER__)

        mesh(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, GLenum mode = GL_TRIANGLES);

#else

		mesh(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, ui32 mode = 0);

#endif
        ~mesh();
        
        ui32 get_id() const;
        
        vbo_shared_ptr get_vbo() const;
        ibo_shared_ptr get_ibo() const;
        
        void bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
        void draw() const;
        void draw(ui32 indices) const;
        void unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
        
        static bool intersect(const vbo_shared_ptr& vbo_01, const ibo_shared_ptr& ibo_01, const glm::mat4& mat_m_01,
                              const vbo_shared_ptr& vbo_02, const ibo_shared_ptr& ibo_02, const glm::mat4& mat_m_02);
    };
};
