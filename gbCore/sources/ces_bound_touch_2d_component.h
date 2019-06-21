//
//  ces_bound_touch_2d_component.h
//  gbCore
//
//  Created by serhii.s on 2/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_bound_touch_component.h"

namespace gb
{
    class ces_bound_touch_2d_component : public ces_bound_touch_component
    {

    private:
        
    protected:
    
        glm::vec4 m_bounds = glm::vec4(0.f);
        
    public:
        
        ces_bound_touch_2d_component();
        ~ces_bound_touch_2d_component();
        
        ces_bound_touch_2d_component(const ces_bound_touch_2d_component& copy) = delete;
        ces_bound_touch_2d_component& operator=(const ces_bound_touch_2d_component& copy) = delete;
        
        glm::vec4 get_bounds() const;
        void set_bounds(const glm::vec4& bounds);
    };
};

