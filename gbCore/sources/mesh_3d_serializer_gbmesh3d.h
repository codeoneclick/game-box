//
//  mesh_3d_serializer_gbmesh3d.h
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_serializer.h"

namespace gb
{
    class mesh_3d_serializer_gbmesh3d final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        mesh_3d_serializer_gbmesh3d(const std::string& filename,
                                    const resource_shared_ptr& resource);
        ~mesh_3d_serializer_gbmesh3d();
        
        void serialize(const resource_transfering_data_shared_ptr& transfering_data);
    };
};
