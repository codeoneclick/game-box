//
//  ces_base_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_base_component_h
#define ces_base_component_h

#include "main_headers.h"
#include "declarations.h"
#include "std_extensions.h"

namespace gb
{
    class ces_base_component : public std::enable_shared_from_this<ces_base_component>
    {
    private:
        
    protected:
        
        static std::set<uintptr_t> g_guids_container;
        ces_base_component();
        
    public:
        
        CTTI_CLASS_GUID(ces_base_component, ces_base_component::g_guids_container)
        virtual ~ces_base_component() = default;
    };
};

#endif
