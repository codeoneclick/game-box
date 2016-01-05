//
//  shader_compiler_glsl.h
//  gbCore
//
//  Created by Sergey Sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef shader_compiler_glsl_h
#define shader_compiler_glsl_h

#include "main_headers.h"

namespace gb
{
    class shader_compiler_glsl
    {
    private:
        
    protected:
        
        static std::string m_vs_shader_header;
        static std::string m_fs_shader_header;
        
    public:
        
        shader_compiler_glsl(void) = default;
        ~shader_compiler_glsl(void) = default;
        
        static ui32 compile(const std::string& source_code, GLenum shader_type, std::string* out_message = nullptr, bool* out_success = nullptr);
        static ui32 link(ui32 vs_handle, ui32 fs_handle, std::string* out_message = nullptr, bool* out_success = nullptr);
    };
};

#endif
