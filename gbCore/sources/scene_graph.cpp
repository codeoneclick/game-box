//
//  scene_graph.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "scene_graph.h"
#include "game_commands_container.h"
#include "ces_transformation_component.h"
#include "ces_transformation_extension.h"
#include "ces_box2d_body_component.h"
#include "ces_box2d_world_component.h"
#include "ces_geometry_component.h"
#include "mesh.h"
#include "vbo.h"

namespace gb
{
    scene_graph::scene_graph(const game_transition_shared_ptr& transition) :
    m_transition(transition),
    m_internal_commands(std::make_shared<game_commands_container>()),
    m_external_commands(nullptr),
    m_fabricator(nullptr),
    m_camera(nullptr)
    {
        ces_transformation_component_shared_ptr transformation_component = std::make_shared<ces_transformation_component>();
        ces_entity::add_component(transformation_component);
        transformation_component->set_z_order(0.f);
    }
    
    scene_graph::~scene_graph()
    {
        
    }
    
    void scene_graph::create()
    {

    }
    
    game_transition_shared_ptr scene_graph::get_transition() const
    {
        return m_transition.lock();
    }
    
    void scene_graph::set_external_commands(const game_commands_container_shared_ptr& commands)
    {
        m_external_commands = commands;
    }
    
    game_commands_container_shared_ptr scene_graph::get_internal_commands() const
    {
        return m_internal_commands;
    }
    
    void scene_graph::set_fabricator(const scene_fabricator_shared_ptr& fabricator)
    {
        m_fabricator = fabricator;
    }
    
    scene_fabricator_shared_ptr scene_graph::get_fabricator() const
    {
        return m_fabricator;
    }
    
    void scene_graph::set_camera(const camera_shared_ptr& camera)
    {
        m_camera = camera;
    }
    
    camera_shared_ptr scene_graph::get_camera() const
    {
        return m_camera;
    }
    
    void scene_graph::updated_z_order_recursively(const ces_entity_shared_ptr& entity, f32& z_order)
    {
        ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
        if(transformation_component)
        {
            transformation_component->set_z_order(z_order);
        }
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            z_order = z_order + ces_transformation_component::k_z_order_step;
            scene_graph::updated_z_order_recursively(child, z_order);
        }
    }
    
    void scene_graph::add_child(const ces_entity_shared_ptr& child)
    {
        ces_entity::add_child(child);
        ces_transformation_component_shared_ptr transformation_component = child->get_component<ces_transformation_component>();
        f32 z_order = 0;
        scene_graph::updated_z_order_recursively(shared_from_this(), z_order);
        ces_transformation_extension::update_absolute_transformation_recursively(child);
    }
    
    void scene_graph::enable_box2d_world(const glm::vec2& min_bound, const glm::vec2& max_bound)
    {
        ces_box2d_world_component_shared_ptr box2d_world_component = ces_entity::get_component<ces_box2d_world_component>();
        if(!box2d_world_component) {
            box2d_world_component = std::make_shared<ces_box2d_world_component>();
            ces_entity::add_component(box2d_world_component);
        }
        box2d_world_component->set_world_bounds(min_bound, max_bound);
    }
    
    void scene_graph::disable_box2d_world()
    {
        ces_box2d_world_component_shared_ptr box2d_world_component = ces_entity::get_component<ces_box2d_world_component>();
        if(box2d_world_component) {
            ces_entity::remove_component(box2d_world_component);
        }
    }
    
    void scene_graph::apply_box2d_physics(const ces_entity_shared_ptr& entity, const custom_setup_box2d_component_t& callback)
    {
        ces_box2d_world_component_shared_ptr box2d_world_component = ces_entity::get_component<ces_box2d_world_component>();
        ces_box2d_body_component_shared_ptr box2d_body_component = entity->get_component<ces_box2d_body_component>();
        
        if(box2d_world_component && !box2d_body_component) {
            box2d_body_component = std::make_shared<ces_box2d_body_component>();
            if(callback)
            {
                callback(box2d_body_component);
            }
            
            ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
            b2BodyDef* box2d_body_definition = box2d_body_component->box2d_body_definition;
            box2d_body_definition->type = b2_dynamicBody;
            glm::vec2 position = transformation_component->get_position();
            box2d_body_definition->position = b2Vec2(position.x, position.y);
            box2d_body_definition->userData = entity.get();
            
            ces_box2d_body_component::e_shape shape = box2d_body_component->shape;
            std::shared_ptr<b2Shape> box2d_shape = nullptr;
            switch (shape) {
                case ces_box2d_body_component::current_geometry_convex:
                {
                    std::shared_ptr<b2PolygonShape> box2d_polygon_shape = std::make_shared<b2PolygonShape>();
                    ces_geometry_component_shared_ptr geometry_component = entity->get_component<ces_geometry_component>();
                    vbo_shared_ptr vbo = geometry_component->get_mesh()->get_vbo();
                    vbo::vertex_attribute *vertices = vbo->lock();
                    std::vector<b2Vec2> points;
                    points.resize(vbo->get_used_size());
                    for(ui32 i = 0; i < vbo->get_used_size(); ++i) {
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
            entity->add_component(box2d_body_component);
        }
    }
    
    void scene_graph::remove_box2d_physics(const ces_entity_shared_ptr& entity)
    {
        ces_box2d_world_component_shared_ptr box2d_world_component = ces_entity::get_component<ces_box2d_world_component>();
        ces_box2d_body_component_shared_ptr box2d_body_component = entity->get_component<ces_box2d_body_component>();
        if(box2d_world_component && box2d_body_component) {
            std::shared_ptr<b2World> box2d_world = box2d_world_component->box2d_world;
            box2d_world->DestroyBody(box2d_body_component->box2d_body);
            box2d_body_component->box2d_body = nullptr;
            entity->remove_component(box2d_body_component);
        }
    }
};
