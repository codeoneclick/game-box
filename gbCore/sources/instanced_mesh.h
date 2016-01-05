//
//  instanced_mesh.h
//  gbCore
//
//  Created by sergey.sergeev on 10/1/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef instanced_mesh_h
#define instanced_mesh_h

#include "mesh.h"

namespace gb
{
    class instanced_mesh : public mesh
    {
    private:
        
    protected:
        
        i32 m_num_instances;
        
    public:
        
        instanced_mesh(const std::string& guid, i32 num_instances = 1, GLenum mode = GL_TRIANGLES);
        
        static std::shared_ptr<instanced_mesh> construct(const std::string& guid,
                                                         const vbo_shared_ptr& vbo,
                                                         const ibo_shared_ptr& ibo,
                                                         const glm::vec3& min_bound,
                                                         const glm::vec3& max_bound,
                                                         i32 num_instances = 1,
                                                         GLenum mode = GL_TRIANGLES);
        
        static std::shared_ptr<instanced_mesh> construct(const std::string& guid,
                                                         const vbo_shared_ptr& vbo,
                                                         const ibo_shared_ptr& ibo,
                                                         i32 num_instances = 1,
                                                         GLenum mode = GL_TRIANGLES);
        ~instanced_mesh();
        
        i32 get_num_instances() const;
        void set_num_instances(i32 num_instances);
        
        void draw() const;
        void draw(ui32 indices) const;
    };
};

#endif
