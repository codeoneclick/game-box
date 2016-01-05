//
//  mesh_serializer_3dmesh.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef mesh_serializer_3dmesh_h
#define mesh_serializer_3dmesh_h

#include "resource_serializer.h"

namespace gb
{
    class mesh_serializer_3dmesh final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        mesh_serializer_3dmesh(const std::string& filename, const resource_shared_ptr& resource);
        ~mesh_serializer_3dmesh();
        
        void serialize();
    };
};

#endif
