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
#include "ces_component_types.h"
#include "ces_component_events.h"

namespace gb
{
    class ces_base_component : public std::enable_shared_from_this<ces_base_component>
    {
    private:
        
    protected:
        ces_base_component();
        
    public:
        
        CTTI_GUID_H(ces_base_component)
        virtual ~ces_base_component() = default;
    };
};

#endif
