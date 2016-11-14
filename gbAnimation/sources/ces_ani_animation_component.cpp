//
//  ces_ani_animation_component.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_ani_animation_component.h"


namespace gb
{
    namespace anim
    {

        ces_ani_animation_component::ces_ani_animation_component() :
        m_timeline(nullptr)
        {
            timeline.setter([=](const std::shared_ptr<ani_timeline>& timeline) {
                m_timeline = timeline;
            });
            timeline.getter([=]() {
                return m_timeline;
            });
        }
        
        ces_ani_animation_component::~ces_ani_animation_component()
        {
            
        }
    }
}

