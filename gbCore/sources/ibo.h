//
//  ibo.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ibo_h
#define ibo_h

#include "resource.h"

namespace gb
{
    class ibo
    {
    private:
        
    protected:
        
        ui32 m_handle;
        ui32 m_version;
        
        ui16* m_data;
        ui32 m_allocated_size;
        ui32 m_used_size;
        
        ui32 m_mode;
        
    public:
        
        ibo(ui32 size, ui32 mode);
        ~ibo();
        
        ui32 get_id() const;
        ui32 get_version() const;
        
        ui32 get_allocated_size() const;
        ui32 get_used_size() const;
        
        ui16* lock() const;
        void unlock(bool is_batching = false, ui32 size = 0);
        
        void bind() const;
        void unbind() const;
    };
};

#endif
