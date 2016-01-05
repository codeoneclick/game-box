//
//  ces_fly_camera_navigation_component.h
//  gbDemo
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_fly_camera_navigation_component_h
#define ces_fly_camera_navigation_component_h

#include "ces_camera_navigation_component.h"

namespace koth
{
    class ces_fly_camera_navigation_component : public ces_camera_navigation_component
    {
    private:
        
    protected:
        
        f32 m_horizontal_angle;
        f32 m_vertical_angle;
        
    public:
        
        ces_fly_camera_navigation_component();
        ~ces_fly_camera_navigation_component();
        
        void update(f32 deltatime);
    };
};

#endif
