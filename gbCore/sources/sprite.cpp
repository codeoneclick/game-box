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
#include "ces_transformation_2d_component.h"
#include "ces_material_component.h"
#include "ces_luminous_component.h"
#include "ces_material_component.h"
#include "mesh_2d.h"
#include "vbo.h"
#include "ibo.h"
#include "glm_extensions.h"

namespace gb
{
    static const std::string k_color_uniform = "u_color";
    static const std::string k_alpha_uniform = "u_alpha";
    
    sprite::sprite()
    {
#if !defined(__NO_RENDER__)

        auto material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);

#endif
        
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
                
#if !defined(__NO_RENDER__)

                ces_shadow_component_shared_ptr shadow_component = std::make_shared<ces_shadow_component>();
                ces_entity::add_component(shadow_component);

#endif
            }
            else
            {
                ces_entity::remove_component(ces_convex_hull_component::class_guid());

#if !defined(__NO_RENDER__)

				ces_entity::remove_component(ces_shadow_component::class_guid());

#endif

            }
        });
        is_shadow_caster.getter([=]() {

#if !defined(__NO_RENDER__)

            return ces_entity::is_component_exist<ces_shadow_component>();

#else

			return false;

#endif

        });
        
        is_luminous.setter([=](bool value) {

#if !defined(__NO_RENDER__)

            if(value)
            {
                ces_entity::add_component_recursively<ces_luminous_component>();
            }
            else
            {
                ces_entity::remove_component_recursively<ces_luminous_component>();
            }

#endif

        });
        
        is_luminous.getter([=]() {

#if !defined(__NO_RENDER__)

            return ces_entity::is_component_exist<ces_luminous_component>();

#else

			return false;

#endif

        });
        
        color.setter([=](const glm::u8vec4& color) {
            m_color = color;
            auto mesh = geometry_component->get_mesh();
            if(mesh)
            {
                auto vbo = mesh->get_vbo();
                vertex_attribute* vertices = vbo->lock();
                for(i32 i = 0; i < vbo->get_used_size(); ++i)
                {
                    vertices[i].m_color = m_color;
                }
                vbo->unlock(material_component->get_is_batching());
            }
        });
        color.getter([=]() {
            return m_color;
        });
        
        alpha.setter([=](ui8 alpha) {
            m_color.w = alpha;
            auto mesh = geometry_component->get_mesh();
            if(mesh)
            {
                auto vbo = mesh->get_vbo();
                vertex_attribute* vertices = vbo->lock();
                for(i32 i = 0; i < vbo->get_used_size(); ++i)
                {
                    vertices[i].m_color = m_color;
                }
                vbo->unlock(material_component->get_is_batching());
            }
        });
        alpha.getter([=]() {
            return m_color.w;
        });
    }
    
    sprite::~sprite()
    {
        
    }
    
    void sprite::set_custom_texcoord(const glm::vec4& texcoord)
    {
        auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
        geometry_component->update_texcoord(texcoord);
    }
}
