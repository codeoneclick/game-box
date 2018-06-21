//
//  memory_map.h
//  gbCore
//
//  Created by sergey.sergeev on 11/6/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class memory_map
    {
    private:
        
        static i32 g_filedescriptors;

#if defined(__OSX__) || defined(__IOS__) || defined(__TVOS__)

        i32 m_filedescriptor;

#elif defined(__WINOS__)

		HANDLE m_filedescriptor;

#endif
        std::string m_filename;
        void* m_pointer;
        
    protected:
        
    public:
        
        memory_map();
        ~memory_map();
        
        void* allocate(const std::string& filename);
        void deallocate();
        void reallocate();
        
        inline void* pointer() const { return m_pointer; };
    };
};