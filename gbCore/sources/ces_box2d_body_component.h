//
//  ces_box2d_body_component.h
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_box2d_body_component : public ces_base_component
    {
    public:
        
        enum e_shape
        {
            current_geometry = 0,
            custom_geometry,
            circle,
            chain,
            box,
            edge,
            capsule
        };
        
        typedef std::function<void(ces_entity_const_shared_ptr entity, ces_box2d_body_component_const_shared_ptr component)> deferred_box2d_component_setup_t;
        typedef std::function<void(ces_entity_const_shared_ptr entity, ces_box2d_body_component_const_shared_ptr component, b2Body*, std::vector<std::shared_ptr<b2Shape>>)> custom_box2d_body_setup_t;
        
        struct box2d_shape_parameters
        {
            e_shape m_shape;
            
            // custom
            std::vector<b2Vec2> m_custom_vertices;
            
            // circle
            f32 m_radius;
            
            // box
            f32 m_hx = 0.f;
            f32 m_hy = 0.f;
            glm::vec2 m_center = glm::vec2(0.f);
            f32 m_angle = 0.f;
            
            void set_radius(f32 radius);
            void set_custom_vertices(const std::vector<b2Vec2>& vertices);
        };
    
    private:
        
    protected:
        
        bool m_is_applied;
        
        b2Body* m_box2d_body;
        b2BodyDef* m_box2d_body_definition;
        
        std::vector<box2d_shape_parameters> m_shapes_parameters;
        
        bool m_is_contacted;
        bool m_is_destructable_on_contact;
        ces_entity_weak_ptr m_contacted_entity;
        std::list<ces_entity_weak_ptr> m_contact_ignore_list;
        ui32 m_body_entity_guid;
        
        std::tuple<ces_entity_weak_ptr, b2BodyType, deferred_box2d_component_setup_t> m_deferred_box2d_component_setup;
        custom_box2d_body_setup_t m_custom_box2d_body_setup = nullptr;
        
    public:
        
        STTI_CLASS_GUID(ces_box2d_body_component, ces_base_component::g_guids_container, override)
        ces_box2d_body_component();
        ~ces_box2d_body_component();

        std::property_rw<glm::vec2> position;
        std::property_rw<f32> rotation;
        std::property_rw<glm::vec2> linear_velocity;
        std::property_rw<f32> angular_velocity;
        std::property_ro<b2BodyDef*> box2d_body_definition;
        
        std::property_rw<b2Body*> box2d_body;
        
        std::property_rw<bool> is_contacted;
        std::property_rw<bool> is_destructable_on_contact;
        
        std::property_rw<ces_entity_shared_ptr> contacted_entity;
        std::property_rw<ui32> body_entity_guid;
        
        void add_shape_parameters(const box2d_shape_parameters& parameters);
        std::vector<box2d_shape_parameters> get_shapes_parameters() const;
        
        std::property_rw<bool> is_applied;
        std::property_rw<bool> enabled;
        
        void add_to_contact_ignore_list(const ces_entity_shared_ptr& entity);
        std::list<ces_entity_weak_ptr> get_contact_ignore_list() const;
        
        void set_deferred_box2d_component_setup(const ces_entity_shared_ptr& entity,
                                                b2BodyType body = b2BodyType::b2_dynamicBody,
                                                const deferred_box2d_component_setup_t& callback = nullptr);
        const std::tuple<ces_entity_weak_ptr, b2BodyType, deferred_box2d_component_setup_t>& get_deferred_box2d_component_setup() const;
        
        void set_custom_box2d_body_setup(const custom_box2d_body_setup_t& callback);
        custom_box2d_body_setup_t get_custom_box2d_body_setup() const;
        
        bool is_custom_box2d_body_setup_exist() const;
    };
};
