//
//  ces_shadow_emissive_component.h
//  gbCore
//
//  Created by serhii serhiiv on 11/21/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_shadow_emissive_component_h
#define ces_shadow_emissive_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_shadow_emissive_component : public ces_base_component
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_shadow_emissive_component, ces_base_component::g_guids_container)
        ces_shadow_emissive_component();
        ~ces_shadow_emissive_component();
    };
};


#endif
