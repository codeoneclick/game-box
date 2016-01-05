//
//  animation_serializer_seq.h
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef animation_serializer_seq_h
#define animation_serializer_seq_h

#include "resource_serializer.h"

namespace gb
{
    class animation_serializer_seq final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        animation_serializer_seq(const std::string& filename, const resource_shared_ptr& resource);
        ~animation_serializer_seq();
        
        void serialize();
    };
};

#endif
