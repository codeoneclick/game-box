//
//  ces_ani_image_animation_component.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/16/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_ani_image_animation_component.h"

namespace gb
{
    namespace anim
    {
        ces_ani_image_animation_component::ces_ani_image_animation_component() :
        m_object_id_reference(-1),
        m_is_cw90(false)
        {
            object_id_reference.setter([=](i32 object_id_reference) {
                m_object_id_reference = object_id_reference;
            });
            object_id_reference.getter([=]() {
                return m_object_id_reference;
            });
            
            is_cw90.setter([=](bool is_cw90) {
                m_is_cw90 = is_cw90;
            });
            is_cw90.getter([=]() {
                return m_is_cw90;
            });
        }
        
        ces_ani_image_animation_component::~ces_ani_image_animation_component()
        {
            
        }
    }
}
