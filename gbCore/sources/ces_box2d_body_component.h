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
            current_geometry_convex = 0,
            custom_geometry_convex,
            circle
        };
        
        typedef std::function<void(ces_box2d_body_component_const_shared_ptr component)> deferred_box2d_component_setup_t;
        typedef std::function<void(ces_box2d_body_component_const_shared_ptr component, b2Body*, std::shared_ptr<b2Shape>)> custom_box2d_body_setup_t;
    
    private:
        
    protected:
        
        bool m_is_applied;
        
        b2Body* m_box2d_body;
        b2BodyDef* m_box2d_body_definition;
        
        e_shape m_shape;
        std::vector<b2Vec2> m_custom_vertices;
        f32 m_radius;
        
        bool m_is_contacted;
        bool m_is_destructable_on_contact;
        ces_entity_weak_ptr m_contacted_entity;
        ui32 m_body_entity_guid;
        
        std::tuple<ces_entity_weak_ptr, b2BodyType, deferred_box2d_component_setup_t> m_deferred_box2d_component_setup;
        custom_box2d_body_setup_t m_custom_box2d_body_setup = nullptr;
        
    public:
        
        CTTI_CLASS_GUID(ces_box2d_body_component, ces_base_component::g_guids_container)
        ces_box2d_body_component();
        ~ces_box2d_body_component();

        std::property_rw<glm::vec2> position;
        std::property_rw<f32> rotation;
        std::property_rw<glm::vec2> velocity;
        std::property_ro<b2BodyDef*> box2d_body_definition;
        
        std::property_rw<b2Body*> box2d_body;
        
        std::property_rw<e_shape> shape;
        
        std::property_rw<bool> is_contacted;
        std::property_rw<bool> is_destructable_on_contact;
        
        std::property_rw<ces_entity_shared_ptr> contacted_entity;
        std::property_rw<ui32> body_entity_guid;
        
        void set_radius(f32 radius);
        f32 get_radius() const;
        
        void set_custom_vertices(const std::vector<b2Vec2>& vertices);
        const std::vector<b2Vec2>& get_custom_vertices() const;
        
        std::property_rw<bool> is_applied;
        std::property_rw<bool> enabled;
        
        void set_deferred_box2d_component_setup(const ces_entity_shared_ptr& entity,
                                                b2BodyType body = b2BodyType::b2_dynamicBody,
                                                const deferred_box2d_component_setup_t& callback = nullptr);
        const std::tuple<ces_entity_weak_ptr, b2BodyType, deferred_box2d_component_setup_t>& get_deferred_box2d_component_setup() const;
        
        void set_custom_box2d_body_setup(const custom_box2d_body_setup_t& callback);
        custom_box2d_body_setup_t get_custom_box2d_body_setup() const;
        
        bool is_custom_box2d_body_setup_exist() const;
    };
};
