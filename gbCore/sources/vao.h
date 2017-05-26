//
//  vao.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef vao_h
#define vao_h

#include "shader.h"

namespace gb
{
    class vao
    {
    private:
        
        friend class ogl_graveyard_controller;
        
    protected:
        
        static std::mutex m_graveyard_mutex;
        static std::queue<ui32> m_handlers_graveyard;
        void add_to_graveyard(ui32 handler);
        
        ui32 m_handle;
        
    public:
        
        vao();
        ~vao();
        
        static void bind(const std::shared_ptr<vao>& state);
        static void unbind();
    };
};
#endif
