//
//  ces_bound_touch_3d_component.hpp
//  gbCore
//
//  Created by serhii.s on 2/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_bound_touch_component.h"

namespace gb
{
    class ces_bound_touch_3d_component : public ces_bound_touch_component
    {
        
    private:
        
    protected:
        
        glm::vec3 m_min_bound = glm::vec3(0.f);
        glm::vec3 m_max_bound = glm::vec3(0.f);
        
    public:
        
        ces_bound_touch_3d_component();
        ~ces_bound_touch_3d_component();
        
        ces_bound_touch_3d_component(const ces_bound_touch_3d_component& copy) = delete;
        ces_bound_touch_3d_component& operator=(const ces_bound_touch_3d_component& copy) = delete;
        
        glm::vec3 get_min_bound() const;
        glm::vec3 get_max_bound() const;
        void set_min_bound(const glm::vec3& bound);
        void set_max_bound(const glm::vec3& bound);
    };
};
