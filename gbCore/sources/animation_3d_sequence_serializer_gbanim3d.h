//
//  animation_sequence_3d_serializer_gbanim3d.h
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_serializer.h"

namespace gb
{
    class animation_sequence_3d_serializer_gbanim3d final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        animation_sequence_3d_serializer_gbanim3d(const std::string& filename, const resource_shared_ptr& resource);
        ~animation_sequence_3d_serializer_gbanim3d();
        
        void serialize(const resource_transfering_data_shared_ptr& transfering_data);
    };

};
