//
//  ces_box2d_body_component.h
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_box2d_body_component_h
#define ces_box2d_body_component_h

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
    
    private:
        
    protected:
        
        glm::vec2 m_position;
        f32 m_rotation;
        
        b2Body* m_box2d_body;
        b2BodyDef* m_box2d_body_definition;
        
        e_shape m_shape;
        std::vector<b2Vec2> m_custom_vertices;
        f32 m_radius;
        
    public:
        
        CTTI_CLASS_GUID(ces_box2d_body_component, ces_base_component::g_guids_container)
        ces_box2d_body_component();
        ~ces_box2d_body_component();

        std::property_ro<glm::vec2> position;
        std::property_ro<f32> rotation;
        std::property_ro<b2BodyDef*> box2d_body_definition;
        
        std::property_rw<b2Body*> box2d_body;
        
        std::property_rw<e_shape> shape;
        
        void set_radius(f32 radius);
        f32 get_radius() const;
        
        void set_custom_vertices(const std::vector<b2Vec2>& vertices);
        const std::vector<b2Vec2>& get_custom_vertices() const;
        
        void on_position_changed(const glm::vec2& position);
        void on_rotation_changed(f32 rotation);
        
    };
};

#endif 
