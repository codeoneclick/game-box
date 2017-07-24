//
//  ces_box2d_system.cpp
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_system.h"
#include "ces_box2d_body_component.h"
#include "ces_box2d_world_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_transformation_extension.h"
#include "ces_geometry_component.h"
#include "mesh_2d.h"
#include "vbo.h"

namespace gb
{
    ces_box2d_system::ces_box2d_system()
    {
        ces_base_system::add_required_component_guid(m_box2d_components_mask, ces_box2d_body_component::class_guid());
        ces_base_system::add_required_components_mask(m_box2d_components_mask);
    }
    
    ces_box2d_system::~ces_box2d_system()
    {
        
    }
    
    void ces_box2d_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_box2d_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        auto box2d_world_component = root->get_component<ces_box2d_world_component>();
        if (box2d_world_component)
        {
            std::shared_ptr<b2World> box2d_world = box2d_world_component->box2d_world;
            box2d_world->Step(1.f / 60.f, 1, 1);
            
            ces_base_system::enumerate_entities_with_components(m_box2d_components_mask, [box2d_world_component](const ces_entity_shared_ptr& entity) {
                
                auto box2d_body_component = entity->get_component<ces_box2d_body_component>();
                if(!box2d_body_component->is_applied)
                {
                    auto deferred_box2d_apply = box2d_body_component->get_deferred_box2d_apply();
                    if(std::get<2>(deferred_box2d_apply))
                    {
                        std::get<2>(deferred_box2d_apply)(box2d_body_component);
                    }
                    
                    auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                    b2BodyDef* box2d_body_definition = box2d_body_component->box2d_body_definition;
                    box2d_body_definition->type = std::get<1>(deferred_box2d_apply);
                    
                    ces_box2d_body_component::e_shape shape = box2d_body_component->shape;
                    std::shared_ptr<b2Shape> box2d_shape = nullptr;
                    switch (shape) {
                        case ces_box2d_body_component::current_geometry_convex:
                        {
                            std::shared_ptr<b2PolygonShape> box2d_polygon_shape = std::make_shared<b2PolygonShape>();
                            ces_geometry_component_shared_ptr geometry_component = entity->get_component<ces_geometry_component>();
                            vbo_shared_ptr vbo = geometry_component->get_mesh()->get_vbo();
                            vbo::vertex_attribute_P *vertices = vbo->lock<vbo::vertex_attribute_P>();
                            std::vector<b2Vec2> points;
                            points.resize(vbo->get_used_size());
                            for(ui32 i = 0; i < vbo->get_used_size(); ++i)
                            {
                                points[i] = b2Vec2(vertices[i].m_position.x, vertices[i].m_position.y);
                            }
                            box2d_polygon_shape->Set(points.data(), static_cast<i32>(points.size()));
                            box2d_shape = box2d_polygon_shape;
                        }
                            break;
                        case ces_box2d_body_component::custom_geometry_convex:
                        {
                            std::shared_ptr<b2PolygonShape> box2d_polygon_shape = std::make_shared<b2PolygonShape>();
                            std::vector<b2Vec2> points = box2d_body_component->get_custom_vertices();
                            box2d_polygon_shape->Set(points.data(), static_cast<i32>(points.size()));
                            box2d_shape = box2d_polygon_shape;
                        }
                            break;
                        case ces_box2d_body_component::circle:
                        {
                            std::shared_ptr<b2CircleShape> box2d_circle_shape = std::make_shared<b2CircleShape>();
                            f32 radius = box2d_body_component->get_radius();
                            box2d_circle_shape->m_radius = radius;
                            box2d_shape = box2d_circle_shape;
                        }
                            break;
                        default:
                            break;
                    }
                    
                    std::shared_ptr<b2World> box2d_world = box2d_world_component->box2d_world;
                    b2Body* box2d_body = box2d_world->CreateBody(box2d_body_component->box2d_body_definition);
                    box2d_body->CreateFixture(box2d_shape.get(), 1);
                    box2d_body_component->box2d_body = box2d_body;
                    box2d_body_component->body_entity_guid = box2d_world_component->register_box2d_body_entity(entity);
                    
                    glm::vec2 position = transformation_component->get_position();
                    f32 rotation = transformation_component->get_rotation();
                    box2d_body_component->position = position;
                    box2d_body_component->rotation = rotation;
                    box2d_body_component->is_applied = true;
                }
                else
                {
                    if(box2d_body_component->is_contacted && box2d_body_component->is_destructable_on_contact)
                    {
                        box2d_world_component->unregister_box2d_body_entity(box2d_body_component->body_entity_guid);
                    }
                    else
                    {
                        b2BodyDef* body_definition = box2d_body_component->box2d_body_definition;
                        if(body_definition->type != b2_staticBody)
                        {
                            auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                            transformation_component->set_position(box2d_body_component->position);
                            transformation_component->set_rotation(box2d_body_component->rotation);
                            ces_transformation_extension::update_absolute_transformation_recursively(entity);
                        }
                    }
                }
            });
        }
    }
    
    void ces_box2d_system::on_feed_end(f32 dt)
    {
        
    }
}
