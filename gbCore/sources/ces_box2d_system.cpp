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
#include "ces_transformation_3d_component.h"
#include "ces_transformation_extension.h"
#include "ces_geometry_component.h"
#include "mesh_2d.h"
#include "vbo.h"
#include "game_loop.h"

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
            
            if (box2d_world->GetBodyCount() > 0)
            {
                // dt = glm::clamp(dt, 0.f, .25f);
                
                f32 maximum_step = 1.f / 60.f;
                // f32 accumulator = 0.f;
                
                // while (accumulator < dt)
                {
                    // f32 step = std::min((dt - accumulator), maximum_step);
                    box2d_world->Step(maximum_step * box2d_world_component->get_update_interval(), 1, 1);
                    // accumulator += step;
                }
            }
            
            ces_base_system::enumerate_entities_with_components(m_box2d_components_mask, [box2d_world_component](const ces_entity_shared_ptr& entity) {
                
                auto box2d_body_component = entity->get_component<ces_box2d_body_component>();
                if(!box2d_body_component->is_applied)
                {
                    auto deferred_box2d_component_setup = box2d_body_component->get_deferred_box2d_component_setup();
                    if(std::get<2>(deferred_box2d_component_setup))
                    {
                        std::get<2>(deferred_box2d_component_setup)(entity, box2d_body_component);
                    }
                    
                    auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                    b2BodyDef* box2d_body_definition = box2d_body_component->box2d_body_definition;
                    box2d_body_definition->type = std::get<1>(deferred_box2d_component_setup);
                    
                    std::vector<std::shared_ptr<b2Shape>> box2d_shapes;
                    const auto box2d_shapes_parameters = box2d_body_component->get_shapes_parameters();
                    for (const auto& shape_parameters_it : box2d_shapes_parameters)
                    {
                        ces_box2d_body_component::e_shape shape = shape_parameters_it.m_shape;
                        switch (shape) {
                            case ces_box2d_body_component::current_geometry:
                            {
                                std::shared_ptr<b2PolygonShape> shape = std::make_shared<b2PolygonShape>();
                                ces_geometry_component_shared_ptr geometry_component = entity->get_component<ces_geometry_component>();
                                vbo_shared_ptr vbo = geometry_component->get_mesh()->get_vbo();
                                vbo::vertex_attribute_P *vertices = vbo->lock<vbo::vertex_attribute_P>();
                                std::vector<b2Vec2> points;
                                points.resize(vbo->get_used_size());
                                for(ui32 i = 0; i < vbo->get_used_size(); ++i)
                                {
                                    points[i] = b2Vec2(vertices[i].m_position.x, vertices[i].m_position.y);
                                }
                                shape->Set(points.data(), static_cast<i32>(points.size()));
                                box2d_shapes.push_back(shape);
                            }
                                break;
                            case ces_box2d_body_component::custom_geometry:
                            {
                                std::shared_ptr<b2PolygonShape> shape = std::make_shared<b2PolygonShape>();
                                std::vector<b2Vec2> points = shape_parameters_it.m_custom_vertices;
                                shape->Set(points.data(), static_cast<i32>(points.size()));
                                box2d_shapes.push_back(shape);
                            }
                                break;
                            case ces_box2d_body_component::circle:
                            {
                                std::shared_ptr<b2CircleShape> shape = std::make_shared<b2CircleShape>();
                                shape->m_radius = shape_parameters_it.m_radius;
                                box2d_shapes.push_back(shape);
                            }
                                break;
                                
                            case ces_box2d_body_component::chain:
                            {
                                std::shared_ptr<b2ChainShape> shape = std::make_shared<b2ChainShape>();
                                const auto vertices = shape_parameters_it.m_custom_vertices;
                                shape->CreateChain(&vertices[0], static_cast<i32>(vertices.size()));
                                box2d_shapes.push_back(shape);
                            }
                                break;
                                
                            case ces_box2d_body_component::box:
                            {
                                std::shared_ptr<b2PolygonShape> shape = std::make_shared<b2PolygonShape>();
                                shape->SetAsBox(shape_parameters_it.m_hx,
                                                shape_parameters_it.m_hy,
                                                b2Vec2(shape_parameters_it.m_center.x,
                                                       shape_parameters_it.m_center.y),
                                                shape_parameters_it.m_angle);
                                box2d_shapes.push_back(shape);
                            }
                                break;
                                
                            case ces_box2d_body_component::edge:
                            {
                                std::shared_ptr<b2EdgeShape> shape = std::make_shared<b2EdgeShape>();
                                shape->Set(shape_parameters_it.m_custom_vertices.at(0),
                                           shape_parameters_it.m_custom_vertices.at(1));
                                box2d_shapes.push_back(shape);
                            }
                                break;
                                
                            default:
                                break;
                        }
                    }
                    
                    std::shared_ptr<b2World> box2d_world = box2d_world_component->box2d_world;
                    b2Body* box2d_body = box2d_world->CreateBody(box2d_body_component->box2d_body_definition);
                    
                    if (box2d_body_component->is_custom_box2d_body_setup_exist())
                    {
                        auto custom_box2d_body_setup = box2d_body_component->get_custom_box2d_body_setup();
                        custom_box2d_body_setup(entity, box2d_body_component, box2d_body, box2d_shapes);
                    }
                    else
                    {
                        for (const auto& shape_it : box2d_shapes)
                        {
                            box2d_body->CreateFixture(shape_it.get(), 1.f);
                        }
                    }
                   
                    box2d_body_component->box2d_body = box2d_body;
                    box2d_body_component->body_entity_guid = box2d_world_component->register_box2d_body_entity(entity);
                    
                    if (entity->get_component<ces_transformation_component>()->is_2d())
                    {
                        auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                        auto current_position = transformation_component->get_position();
                        auto current_rotation = transformation_component->get_rotation();
                        box2d_body_component->position = current_position;
                        box2d_body_component->rotation = current_rotation;
                    }
                    else if (entity->get_component<ces_transformation_component>()->is_3d())
                    {
                        auto transformation_component = entity->get_component<ces_transformation_3d_component>();
                        auto current_position = transformation_component->get_position();
                        auto current_rotation = transformation_component->get_rotation();
                        box2d_body_component->position = glm::vec2(current_position.x, current_position.z);
                        box2d_body_component->rotation = current_rotation.y;
                    }
                    else
                    {
                        assert(false);
                    }
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
                            if (entity->get_component<ces_transformation_component>()->is_2d())
                            {
                                auto transformation_component = entity->get_component<ces_transformation_2d_component>();
                                transformation_component->set_position(box2d_body_component->position);
                                transformation_component->set_rotation(box2d_body_component->rotation);
                            }
                            else if (entity->get_component<ces_transformation_component>()->is_3d())
                            {
                                auto transformation_component = entity->get_component<ces_transformation_3d_component>();
                                auto current_position = transformation_component->get_position();
                                auto current_rotation = transformation_component->get_rotation();
                                glm::vec2 box2d_position = box2d_body_component->position;
                                f32 box2d_rotation = box2d_body_component->rotation;
                                current_position.x = box2d_position.x;
                                current_position.z = box2d_position.y;
                                current_rotation.y = glm::degrees(box2d_rotation);
                                transformation_component->set_position(current_position);
                                transformation_component->set_rotation(current_rotation);
                            }
                            else
                            {
                                assert(false);
                            }
                           
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
