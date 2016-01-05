//
//  mesh_commiter_3dmesh.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef mesh_commiter_3dmesh_h
#define mesh_commiter_3dmesh_h

#include "resource_commiter.h"

namespace gb
{
    class mesh_commiter_3dmesh final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        mesh_commiter_3dmesh(const std::string& guid, const resource_shared_ptr& resource);
        ~mesh_commiter_3dmesh();
        
        void commit();
    };
};

#endif
