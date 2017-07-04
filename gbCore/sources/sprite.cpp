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
    sprite::sprite(bool is_use_batch)
    {
        size.setter([=](const glm::vec2& size) {
            auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
            geometry_component->set_size(size);
        });
        size.getter([=]() {
            auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
            return geometry_component->get_size();
        });
        
        pivot.setter([=](const glm::vec2& pivot) {
            auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
            geometry_component->set_pivot(pivot);
        });
        pivot.getter([=]() {
            auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
            return geometry_component->get_pivot();
        });
        
        is_shadow_caster.setter([=](bool value) {
            if(value)
            {
                auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
                auto convex_hull_component = std::make_shared<ces_convex_hull_component>();
                convex_hull_component->create(geometry_component->get_mesh()->get_vbo()->lock<vbo::vertex_attribute_PTC>(), geometry_component->get_mesh()->get_vbo()->get_used_size());
                ces_entity::add_component(convex_hull_component);
                
#if !defined(__NO_RENDER__)

                auto shadow_component = std::make_shared<ces_shadow_component>();
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
        
        color.setter([=](const glm::u8vec4& color) {
            m_color = color;
            auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
            auto mesh = geometry_component->get_mesh();
            if(mesh)
            {
                auto vbo = mesh->get_vbo();
                vbo::vertex_attribute_PTC* vertices = vbo->lock<vbo::vertex_attribute_PTC>();
                for(i32 i = 0; i < vbo->get_used_size(); ++i)
                {
                    vertices[i].m_color = m_color;
                }
                vbo->unlock();
            }
        });
        color.getter([=]() {
            return m_color;
        });
        
        alpha.setter([=](ui8 alpha) {
            m_color.w = alpha;
            auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
            auto mesh = geometry_component->get_mesh();
            if(mesh)
            {
                auto vbo = mesh->get_vbo();
                vbo::vertex_attribute_PTC* vertices = vbo->lock<vbo::vertex_attribute_PTC>();
                for(i32 i = 0; i < vbo->get_used_size(); ++i)
                {
                    vertices[i].m_color = m_color;
                }
                vbo->unlock();
            }
        });
        alpha.getter([=]() {
            return m_color.w;
        });
    }
    
    sprite::~sprite()
    {
        
    }
    
    sprite_shared_ptr sprite::construct(bool is_use_batch)
    {
        auto entity = std::make_shared<sprite>(is_use_batch);
#if !defined(__NO_RENDER__)
        
        auto material_component = std::make_shared<ces_material_component>();
        entity->add_component(material_component);
        
#endif
        
        auto geometry_component = std::make_shared<ces_geometry_quad_component>(is_use_batch);
        entity->add_component(geometry_component);
        
        return entity;
    }
    
    void sprite::set_custom_texcoord(const glm::vec4& texcoord)
    {
        auto geometry_component = ces_entity::get_component<ces_geometry_quad_component>();
        geometry_component->update_texcoord(texcoord);
    }
}
