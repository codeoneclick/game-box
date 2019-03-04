//
//  ogl_graveyard_controller.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/26/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ogl_graveyard_controller.h"
#include "vbo.h"
#include "ibo.h"
#include "vao.h"

namespace gb
{
    ogl_graveyard_controller::ogl_graveyard_controller()
    {
        
    }
    
    ogl_graveyard_controller::~ogl_graveyard_controller()
    {
        
    }
    
    void ogl_graveyard_controller::update(f32 dt)
    {
        std::lock_guard<std::mutex> guard(vbo::m_graveyard_mutex);
        while(!vbo::m_handlers_graveyard.empty())
        {
            auto handler = vbo::m_handlers_graveyard.front();
            gl::command::delete_buffers(1, &handler);
            vbo::m_handlers_graveyard.pop();
        }
        
        while(!ibo::m_handlers_graveyard.empty())
        {
            auto handler = ibo::m_handlers_graveyard.front();
            gl::command::delete_buffers(1, &handler);
            ibo::m_handlers_graveyard.pop();
        }
        
        while(!vao::m_handlers_graveyard.empty())
        {
            auto handler = vao::m_handlers_graveyard.front();
            gl::command::delete_vertex_arrays(1, &handler);
            vao::m_handlers_graveyard.pop();
        }
    }
}
