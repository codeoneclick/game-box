//
//  scene_graph.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "scene_graph.h"
#include "game_commands_container.h"
#include "ces_transformation_2d_component.h"
#include "ces_transformation_extension.h"
#include "ces_box2d_body_component.h"
#include "ces_box2d_world_component.h"
#include "ces_geometry_component.h"
#include "ces_geometry_freeform_component.h"
#include "mesh_2d.h"
#include "vbo.h"

namespace gb
{
    scene_graph::scene_graph(const game_transition_shared_ptr& transition) :
    m_transition(transition),
    m_internal_commands(std::make_shared<game_commands_container>()),
    m_external_commands(nullptr),
    m_fabricator(nullptr),
    m_camera_2d(nullptr),
    m_camera_3d(nullptr),
    m_scene_size(0.f)
    {
        m_is_root = true;
        ces_entity::add_deferred_component_constructor<ces_transformation_2d_component>();
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
    
    void scene_graph::set_camera_2d(const camera_2d_shared_ptr& camera)
    {
        m_camera_2d = camera;
    }
    
    camera_2d_shared_ptr scene_graph::get_camera_2d() const
    {
        return m_camera_2d;
    }
    
    void scene_graph::set_camera_3d(const camera_3d_shared_ptr& camera)
    {
        m_camera_3d = camera;
    }
    
    camera_3d_shared_ptr scene_graph::get_camera_3d() const
    {
        return m_camera_3d;
    }
    
    void scene_graph::updated_z_order_recursively(const ces_entity_shared_ptr& entity, f32& z_order)
    {
        auto transformation_component = entity->get_component<ces_transformation_2d_component>();
        if(transformation_component && !transformation_component->get_is_custom_z_order())
        {
            transformation_component->set_z_order(z_order);
        }
        else if(transformation_component && transformation_component->get_is_custom_z_order())
        {
            z_order += transformation_component->get_z_order();
        }
        std::vector<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            z_order = z_order + ces_transformation_2d_component::k_z_order_step;
            scene_graph::updated_z_order_recursively(child, z_order);
        }
    }
    
    void scene_graph::add_child(const ces_entity_shared_ptr& child)
    {
        ces_entity::add_child(child);
        auto transformation_component = child->get_component<ces_transformation_2d_component>();
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
    
    void scene_graph::apply_box2d_physics(const ces_entity_shared_ptr& entity, b2BodyType body, const custom_setup_box2d_component_t& callback)
    {
        ces_box2d_world_component_shared_ptr box2d_world_component = ces_entity::get_component<ces_box2d_world_component>();
        ces_box2d_body_component_shared_ptr box2d_body_component = entity->get_component<ces_box2d_body_component>();
        
        if(box2d_world_component && !box2d_body_component) {
            box2d_body_component = std::make_shared<ces_box2d_body_component>();
            if(callback)
            {
                callback(box2d_body_component);
            }

            auto transformation_component = entity->get_component<ces_transformation_2d_component>();
            b2BodyDef* box2d_body_definition = box2d_body_component->box2d_body_definition;
            box2d_body_definition->type = body;
            box2d_body_definition->userData = entity.get();
            
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
            box2d_body_component->body_entity_guid = box2d_world_component->register_box2d_body_entity(entity);
            
            glm::vec2 position = transformation_component->get_position();
            f32 rotation = transformation_component->get_rotation();
            box2d_body_component->position = position;
            box2d_body_component->rotation = rotation;
            
            entity->add_component(box2d_body_component);
        }
    }
    
    void scene_graph::remove_box2d_physics(const ces_entity_shared_ptr& entity)
    {
        ces_box2d_world_component_shared_ptr box2d_world_component = ces_entity::get_component<ces_box2d_world_component>();
        ces_box2d_body_component_shared_ptr box2d_body_component = entity->get_component<ces_box2d_body_component>();
        if(box2d_world_component && box2d_body_component)
        {
            std::shared_ptr<b2World> box2d_world = box2d_world_component->box2d_world;
            box2d_world->DestroyBody(box2d_body_component->box2d_body);
            
            box2d_world_component->unregister_box2d_body_entity(box2d_body_component->body_entity_guid);
            box2d_body_component->body_entity_guid = 0;
            box2d_body_component->box2d_body = nullptr;
            
            entity->remove_component(box2d_body_component);
        }
    }
    
    void scene_graph::disassembly_scene_recursively(const ces_entity_shared_ptr& entity,
                                                    std::map<f32, std::string>& scene_graph_metadata)
    {
        std::string tag = entity->tag;
        f32 z_order = 0.f;
        auto transformation_component = entity->get_component<ces_transformation_2d_component>();
        if(transformation_component)
        {
            z_order = transformation_component->get_z_order();
        }
        scene_graph_metadata.insert(std::make_pair(z_order, tag));
        std::vector<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            scene_graph::disassembly_scene_recursively(child, scene_graph_metadata);
        }
    }
    
    void scene_graph::set_systems_feeder(const ces_systems_feeder_shared_ptr& systems_feeder)
    {
        m_systems_feeder = systems_feeder;
    }
    
    std::map<f32, std::string> scene_graph::disassembly_scene(const ces_entity_shared_ptr& root)
    {
        std::map<f32, std::string> scene_graph_metadata;
        scene_graph::disassembly_scene_recursively(root, scene_graph_metadata);
        return scene_graph_metadata;
    }

};
