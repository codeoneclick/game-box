//
//  simple_shape.h
//  gbCore
//
//  Created by serhii serhiiv on 3/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "vbo.h"
#include "ibo.h"
#include "glm_extensions.h"

namespace gb
{
    class simple_shape
    {

    private:
        
    protected:
        
        std::vector<glm::triangle> m_triangles;
        
    public:
        
        simple_shape(const vbo_shared_ptr& vbo, const ibo_shared_ptr& ibo, const glm::mat4& mat = glm::mat4(1.f));
        ~simple_shape();
        
        static bool intersection(const vbo_shared_ptr& vbo_01, const ibo_shared_ptr& ibo_01, const glm::mat4& mat_01,
                                 const vbo_shared_ptr& vbo_02, const ibo_shared_ptr& ibo_02, const glm::mat4& mat_02);
    };
};

