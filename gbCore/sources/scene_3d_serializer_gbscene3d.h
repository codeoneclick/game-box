//
//  scene_3d_serializer_gbscene3d.hpp
//  gbCore
//
//  Created by serhii.s on 12/17/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_serializer.h"

namespace gb
{
    class scene_3d_serializer_gbscene3d final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        scene_3d_serializer_gbscene3d(const std::string& filename, const resource_shared_ptr& resource);
        ~scene_3d_serializer_gbscene3d();
        
        void serialize(const resource_transfering_data_shared_ptr& transfering_data);
    };
    
};
