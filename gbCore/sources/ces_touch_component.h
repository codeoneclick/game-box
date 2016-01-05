//
//  ces_touch_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_touch_component_h
#define ces_touch_component_h

#include "ces_base_component.h"
#include "input_context.h"

namespace gb
{
    class ces_touch_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::array<bool, e_input_state_max> m_responders;
        
    public:
        
        ces_touch_component();
        ~ces_touch_component();
        
        void enable(e_input_state state, bool value);
        bool is_enabled(e_input_state state);
    };
};

#endif
