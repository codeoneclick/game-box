//
//  vbo.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef vbo_h
#define vbo_h

#include "resource.h"
#include "shader.h"

namespace gb
{
    class vbo
    {
    public:
        
        struct vertex_attribute
        {
            glm::vec3 m_position;
            glm::uint32 m_texcoord;
            glm::u8vec4 m_color;
        };
        
    private:
        
    protected:
        
        ui32 m_handle;
        ui32 m_version;
        
        vertex_attribute* m_data;
        ui32 m_allocated_size;
        ui32 m_used_size;
        
        GLenum m_mode;
        
        glm::vec2 m_min_bound;
        glm::vec2 m_max_bound;
        
    public:
        
        vbo(ui32 size, GLenum mode);
        ~vbo();
        
        ui32 get_id() const;
        ui32 get_version() const;
        
        ui32 get_allocated_size() const;
        ui32 get_used_size() const;
        
        vertex_attribute* lock() const;
        void unlock(ui32 size = 0);
        
        glm::vec2 get_min_bound() const;
        glm::vec2 get_max_bound() const;
        
        void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
        void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
    };
};

#endif
