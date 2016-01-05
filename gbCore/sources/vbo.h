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
    class vbo : public resource_transfering_data
    {
    public:
        
        struct bone_data
        {
            i32	m_id;
            f32 m_weigth;
        };
        
        struct vertex_data
        {
            glm::vec3 m_position;
            glm::vec2 m_texcoord;
            glm::vec3 m_normal;
            glm::vec3 m_tangent;
            std::vector<bone_data> m_bones;
        };
        
        struct vertex_attribute
        {
            glm::vec3 m_position;
            glm::uint32 m_texcoord;
            glm::uint32 m_normal;
            glm::uint32 m_tangent;
            glm::u8vec4 m_color;
            glm::u8vec4 m_extra;
        };
        
    private:
        
    protected:
        
        ui32 m_handle;
        bool m_is_mmap;
        
        vertex_attribute* m_data;
        ui32 m_allocated_size;
        ui32 m_used_size;
        
        GLenum m_mode;
        
    public:
        
        vbo(ui32 size, GLenum mode, vertex_attribute* mmap = nullptr);
        ~vbo(void);
        
        ui32 get_allocated_size(void) const;
        ui32 get_used_size(void) const;
        
        vertex_attribute* lock(void) const;
        void unlock(ui32 size = 0);
        
        void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
        void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
    };
};

#endif
