//
//  map_serializer_tmx.hpp
//  gbCore
//
//  Created by serhii.s on 2/13/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_serializer.h"

namespace gb
{
    class scene_2d_serializer_tmx final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        scene_2d_serializer_tmx(const std::string& filename, const resource_shared_ptr& resource);
        ~scene_2d_serializer_tmx();
        
        void serialize(const resource_transfering_data_shared_ptr& transfering_data);
    };
};
