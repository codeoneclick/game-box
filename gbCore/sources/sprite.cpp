//
//  sprite.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "sprite.h"
#include "ces_geometry_quad_component.h"
#include "ces_shadow_component.h"
#include "ces_convex_hull_component.h"
#include "ces_transformation_component.h"
#include "ces_material_component.h"
#include "ces_luminous_component.h"
#include "mesh.h"
#include "glm_extensions.h"

namespace gb
{
    sprite::sprite()
    {
        auto material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);
        
        auto geometry_component = std::make_shared<ces_geometry_quad_component>();
        ces_entity::add_component(geometry_component);
        
        size.setter([=](const glm::vec2& size) {
            geometry_component->set_size(size);
        });
        size.getter([=]() {
            return geometry_component->get_size();
        });
        
        pivot.setter([=](const glm::vec2& pivot) {
            geometry_component->set_pivot(pivot);
        });
        pivot.getter([=]() {
            return geometry_component->get_pivot();
        });
        
        is_shadow_caster.setter([=](bool value) {
            if(value)
            {
                ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<ces_convex_hull_component>();
                convex_hull_component->create(geometry_component->get_mesh()->get_vbo()->lock(), geometry_component->get_mesh()->get_vbo()->get_used_size());
                ces_entity::add_component(convex_hull_component);
                
                ces_shadow_component_shared_ptr shadow_component = std::make_shared<ces_shadow_component>();
                ces_entity::add_component(shadow_component);
            }
            else
            {
                ces_entity::remove_component(ces_shadow_component::class_guid());
                ces_entity::remove_component(ces_convex_hull_component::class_guid());
            }
        });
        is_shadow_caster.getter([=]() {
            return ces_entity::is_component_exist<ces_shadow_component>();
        });
        
        is_luminous.setter([=](bool value) {
            if(value)
            {
                ces_entity::add_component_recursively<ces_luminous_component>();
            }
            else
            {
                ces_entity::remove_component_recursively<ces_luminous_component>();
            }
        });
        
        is_luminous.getter([=]() {
            return ces_entity::is_component_exist<ces_luminous_component>();
        });
    }
    
    sprite::~sprite()
    {
        
    }
    
    void sprite::set_custom_texcoord(const glm::vec4& texcoord)
    {
        auto geometry_component = ces_entity::get_unsafe_component<ces_geometry_quad_component>();
        geometry_component->update_texcoord(texcoord);
    }
}
