//
//  shader_commiter_glsl.h
//  gbCore
//
//  Created by sergey.sergeev on 8/18/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef shader_commiter_glsl_h
#define shader_commiter_glsl_h

#include "resource_commiter.h"

namespace gb
{
    class shader_commiter_glsl : public resource_commiter
    {
    private:
        
    protected:
        
        std::string m_vs_source_code;
        std::string m_fs_source_code;
        std::string m_vs_filename;
        std::string m_fs_filename;
        
        ui32 compile(const std::string& source_code, GLenum shader_type);
        ui32 link(ui32 vs_handle, ui32 fs_handle);
        
    public:
        
        shader_commiter_glsl(const std::string& guid,
                             const std::string& vs_filename,
                             const std::string& fs_filename,
                             const std::string& vs_source_code,
                             const std::string& fs_source_code,
                             const resource_shared_ptr& resource);
        ~shader_commiter_glsl(void);
        
        void commit(void);
    };
};

#endif
