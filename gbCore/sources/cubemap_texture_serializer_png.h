//
//  cubemap_texture_serializer.h
//  gbCore
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_serializer.h"

namespace gb
{
    class cubemap_texture_serializer_png final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        cubemap_texture_serializer_png(const std::string& filename,
                               const resource_shared_ptr& resource);
        ~cubemap_texture_serializer_png() = default;
        
        void serialize(const resource_transfering_data_shared_ptr& transfering_data);
    };
};

