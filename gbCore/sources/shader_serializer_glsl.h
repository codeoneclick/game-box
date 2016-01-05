//
//  shader_serializer_glsl.h
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef shader_serializer_glsl_h
#define shader_serializer_glsl_h

#include "resource_serializer.h"

namespace gb
{
    class shader_serializer_glsl final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_vs_filename;
        std::string m_fs_filename;
        
    public:
        
        shader_serializer_glsl(const std::string& vs_filename,
                               const std::string& fs_filename,
                               const resource_shared_ptr& resource);
        ~shader_serializer_glsl();
        
        void serialize();
    };
};

#endif
