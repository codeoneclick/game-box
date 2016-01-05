//
//  model3d_animated.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "model3d_animated.h"
#include "ces_animation_component.h"
#include "animation_mixer.h"

namespace gb
{
    model3d_animated::model3d_animated()
    {
        ces_animation_component_shared_ptr animation_component = std::make_shared<ces_animation_component>();
        ces_entity::add_component(animation_component);
    }
    
    model3d_animated::~model3d_animated()
    {

    }
    
    void model3d_animated::set_animation_mixer(const animation_mixer_shared_ptr& animation_mixer)
    {
        unsafe_get_animation_component_from_this->set_animation_mixer(animation_mixer);
    }
    
    void model3d_animated::set_animation(const std::string& name)
    {
        unsafe_get_animation_component_from_this->set_animation(name);
    }
    
    void model3d_animated::create_animation_linkage(const model_configuration_shared_ptr& configuration)
    {
        unsafe_get_animation_component_from_this->create_animation_linkage(configuration);
    }
}