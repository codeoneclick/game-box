//
//  texture_serializer_png.h
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef texture_serializer_png_h
#define texture_serializer_png_h

#include "resource_serializer.h"

namespace gb
{
    class texture_serializer_png final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        texture_serializer_png(const std::string& filename,
                               const resource_shared_ptr& resource);
        ~texture_serializer_png(void);
        
        void serialize(void);
    };
};

#endif
