//
//  ces_bound_touch_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_bound_touch_component_h
#define ces_bound_touch_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_bound_touch_component : public ces_base_component
    {
    private:
        
    protected:
        
        glm::vec4 m_frame;
        
    public:
        
        ces_bound_touch_component();
        ~ces_bound_touch_component();
        
        void set_frame(const glm::vec4& frame);
        glm::vec4 get_frame() const;
    };
};

#endif
