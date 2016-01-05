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
    class shader_commiter_glsl final : public resource_commiter
    {
    private:
        
    protected:
        
        ui32 compile(const std::string& source_code, GLenum shader_type);
        ui32 link(ui32 vs_handle, ui32 fs_handle);
        
    public:
        
        shader_commiter_glsl(const std::string& guid, const resource_shared_ptr& resource);
        ~shader_commiter_glsl();
        
        void commit(const resource_transfering_data_shared_ptr& transfering_data);
    };
};

#endif
