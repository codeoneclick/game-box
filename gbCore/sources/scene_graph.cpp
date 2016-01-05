//
//  scene_graph.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "scene_graph.h"
#include "camera.h"
#include "frustum.h"
#include "shadow_cast_light.h"
#include "game_object.h"
#include "omni_light.h"
#include "direction_light.h"
#include "skybox.h"
#include "ocean.h"
#include "ces_systems_feeder.h"
#include "ces_box2d_system.h"
#include "collision_manager.h"
#include "omni_lights_instances_container.h"

namespace gb
{
    scene_graph::scene_graph()
    {

    }
    
    scene_graph::~scene_graph()
    {
        
    }
    
    void scene_graph::on_update(f32 deltatime)
    {
        if(m_shadow_cast_light)
        {
            m_shadow_cast_light->update(deltatime);
        }
    }
    
    void scene_graph::set_main_camera(const camera_shared_ptr& camera)
    {
        m_main_camera = camera;
        m_systems_feeder->add_entity(m_main_camera);
    }
    
    void scene_graph::set_shadow_cast_light(f32 fov, f32 near, f32 far)
    {
        m_shadow_cast_light = std::make_shared<gb::shadow_cast_light>(fov, near, far);;
    }
    
    shadow_cast_light_shared_ptr scene_graph::get_shadow_cast_light() const
    {
        return m_shadow_cast_light;
    }
    
    void scene_graph::add_direction_light(const direction_light_shared_ptr& direction_light)
    {
        direction_light->on_added_to_scene(shared_from_this());
        m_systems_feeder->add_entity(direction_light);
    }
    
    void scene_graph::remove_direction_light(const direction_light_shared_ptr& direction_light)
    {
        direction_light->on_removed_from_scene();
        m_systems_feeder->remove_entity(direction_light);
    }
    
    void scene_graph::add_omni_light(const omni_light_shared_ptr& omni_light)
    {
        omni_light->on_added_to_scene(shared_from_this());
        m_systems_feeder->add_entity(omni_light);
    }
    
    void scene_graph::remove_omni_light(const omni_light_shared_ptr& omni_light)
    {
        omni_light->on_removed_from_scene();
        m_systems_feeder->remove_entity(omni_light);
    }
    
    void scene_graph::add_game_object(const game_object_shared_ptr& game_object)
    {
        game_object->on_added_to_scene(shared_from_this());
        m_systems_feeder->add_entity(game_object);
    }
    
    void scene_graph::remove_game_object(const game_object_shared_ptr& game_object)
    {
        game_object->on_removed_from_scene();
        m_systems_feeder->remove_entity(game_object);
    }
    
    void scene_graph::set_box2d_world(const glm::vec2 &min_bound, const glm::vec2 &max_bound)
    {
        std::shared_ptr<ces_box2d_system> box2d_system = std::static_pointer_cast<ces_box2d_system>(m_systems_feeder->get_system(ces_system_type.ces_system_type_box2d));
        assert(box2d_system);
        box2d_system->set_box2d_world(min_bound, max_bound);
    }
    
    b2Body* scene_graph::add_box2d_body(const std::shared_ptr<b2BodyDef> box2d_body_definition)
    {
        std::shared_ptr<ces_box2d_system> box2d_system = std::static_pointer_cast<ces_box2d_system>(m_systems_feeder->get_system(ces_system_type.ces_system_type_box2d));
        assert(box2d_system);
        return box2d_system->get_collision_manager()->create_box2d_body(box2d_body_definition);
    }
    
    void scene_graph::remove_box2d_body(b2Body* box2d_body)
    {
        std::shared_ptr<ces_box2d_system> box2d_system = std::static_pointer_cast<ces_box2d_system>(m_systems_feeder->get_system(ces_system_type.ces_system_type_box2d));
        assert(box2d_system);
        box2d_system->get_collision_manager()->destroy_box2d_body(box2d_body);
    }
}