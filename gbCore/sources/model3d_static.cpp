//
//  model3d_static.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "model3d_static.h"
#include "ces_geometry_component.h"
#include "ces_touch_component.h"
#include "ces_bounding_box_debug_component.h"
#include "ces_box2d_component.h"
#include "ces_batch_component.h"
#include "mesh_constructor.h"
#include "mesh.h"
#include "scene_graph.h"

namespace gb
{
    model3d_static::model3d_static() :
    m_physics_cached_parameters(std::make_pair(false, false))
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_component>();
        ces_entity::add_component(geometry_component);
    }
    
    model3d_static::~model3d_static()
    {

    }
    
    void model3d_static::set_mesh(const mesh_shared_ptr& mesh)
    {
        unsafe_get_geometry_component_from_this->set_mesh(mesh);
        
        if(ces_entity::is_component_exist(e_ces_component_type_debug_render))
        {
            mesh_shared_ptr mesh_bounding_box = mesh_constructor::create_wireframe_box(mesh->get_min_bound(),
                                                                                       mesh->get_max_bound());
            unsafe_get_debug_render_component_from_this->set_mesh(mesh_bounding_box);
        }
    }
    
    void model3d_static::set_touches_receives_enabled(bool value)
    {
        if(value)
        {
            ces_touch_component_shared_ptr touch_component = std::make_shared<ces_touch_component>();
            ces_entity::add_component(touch_component);
            touch_component->set_scene_graph_parameters(game_object::get_scene_graph());
            
            touch_component->enable(e_input_state_pressed, true);
            touch_component->enable(e_input_state_dragged, true);
            touch_component->enable(e_input_state_released, true);
        }
        else
        {
            ces_entity::remove_component(e_ces_component_type_touch);
        }
    }
    
    void model3d_static::set_bounding_box_debug_enabled(bool value)
    {
        if(value)
        {
            ces_bounding_box_debug_component_shared_ptr bounding_box_debug_component = std::make_shared<ces_bounding_box_debug_component>();
            ces_entity::add_component(bounding_box_debug_component);
            bounding_box_debug_component->set_scene_graph_parameters(game_object::get_scene_graph());
        }
        else
        {
            ces_entity::remove_component(e_ces_component_type_debug_render);
        }
    }
    
    void model3d_static::create_physics_component()
    {
        ces_box2d_component_shared_ptr box2d_component = std::make_shared<ces_box2d_component>();
        ces_entity::add_component(box2d_component);
        std::shared_ptr<b2BodyDef> body_definition = box2d_component->get_box2d_body_definition();
        
        body_definition->type = m_physics_cached_parameters.second ? b2_staticBody : b2_dynamicBody;
        body_definition->position.Set(game_object::get_position().x, game_object::get_position().z);
        body_definition->userData = this;
        
        b2PolygonShape box2d_shape;
        glm::vec3 min_bound = unsafe_get_geometry_component_from_this->get_mesh()->get_min_bound() * game_object::get_scale();
        glm::vec3 max_bound = unsafe_get_geometry_component_from_this->get_mesh()->get_max_bound() * game_object::get_scale();
        
        box2d_shape.SetAsBox((max_bound.x - min_bound.x) / 2.f,
                             (max_bound.z - min_bound.z) / 2.f);
        
        b2Body* box2d_body = game_object::get_scene_graph()->add_box2d_body(body_definition);
        assert(box2d_body);
        box2d_body->CreateFixture(&box2d_shape, 1);
        box2d_component->set_box2d_body(box2d_body);
    }
    
    void model3d_static::set_enable_box2d_physics(bool value, bool is_static)
    {
        m_physics_cached_parameters.first = value;
        m_physics_cached_parameters.second = is_static;
        
        if(value)
        {
            if(unsafe_get_geometry_component_from_this->get_mesh() && unsafe_get_geometry_component_from_this->get_mesh()->is_loaded())
            {
                model3d_static::create_physics_component();
            }
            else if(unsafe_get_geometry_component_from_this->get_mesh())
            {
                unsafe_get_geometry_component_from_this->get_mesh()->add_resource_loading_callback(std::make_shared<resource::f_resource_loading_callback>([this](const resource_shared_ptr& resource, bool success){
                    model3d_static::create_physics_component();
                }));
            }
            else
            {
                assert(false);
            }
        }
        else
        {
            ces_entity::remove_component(e_ces_component_type_box2d);
        }
    }
    
    void model3d_static::set_is_batched(bool value, const std::string& guid)
    {
        if(value)
        {
            ces_batch_component_shared_ptr batch_component = std::make_shared<ces_batch_component>(guid);
            ces_entity::add_component(batch_component);
            batch_component->set_scene_graph_parameters(game_object::get_scene_graph());
        }
        else
        {
            ces_entity::remove_component(e_ces_component_type_batch);
        }
    }
}