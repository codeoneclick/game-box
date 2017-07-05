//
//  shape_3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "shape_3d.h"
#include "ces_material_component.h"
#include "ces_geometry_3d_component.h"
#include "ces_transformation_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_skeleton_3d_component.h"
#include "mesh_3d.h"

namespace gb
{
    shape_3d::shape_3d()
    {
        
#if !defined(__NO_RENDER__)
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_3d_component>();
        ces_entity::add_deferred_component_constructor<ces_animation_3d_mixer_component>();
        ces_entity::add_deferred_component_constructor<ces_skeleton_3d_component>();
        
        min_bound.getter([=]() {
            auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
            auto mesh = std::static_pointer_cast<mesh_3d>(geometry_component->get_mesh());
            return mesh->get_min_bound();
        });
        
        max_bound.getter([=]() {
            auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
            auto mesh = std::static_pointer_cast<mesh_3d>(geometry_component->get_mesh());
            return mesh->get_max_bound();
        });
    }
    
    shape_3d::~shape_3d()
    {
        
    }
    
    void shape_3d::setup_components()
    {
        auto transformation_component = ces_entity::get_component<ces_transformation_component>();
        transformation_component->set_is_in_camera_space(false);
    }
    
    void shape_3d::play_animation(const std::string& animation_name, bool is_looped)
    {
        auto animation_3d_mixer_component = shape_3d::get_component<ces_animation_3d_mixer_component>();
        animation_3d_mixer_component->set_animation(animation_name, is_looped);
    }
    
    void shape_3d::set_custom_animation_fps(i32 fps)
    {
        auto animation_3d_mixer_component = shape_3d::get_component<ces_animation_3d_mixer_component>();
        animation_3d_mixer_component->set_custom_animation_fps(fps);
    }
}
