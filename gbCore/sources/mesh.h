//
//  mesh.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef mesh_h
#define mesh_h

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
        
        GLenum m_mode;
        
    public:
        
        mesh(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, GLenum mode = GL_TRIANGLES);
        ~mesh();
        
        ui32 get_id() const;
        
        vbo_shared_ptr get_vbo() const;
        ibo_shared_ptr get_ibo() const;
        
        void bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
        void draw() const;
        void draw(ui32 indices) const;
        void unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
    };
};

#endif
