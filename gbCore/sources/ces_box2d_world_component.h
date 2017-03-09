//
//  ces_box2d_world_component.h
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_box2d_world_component_h
#define ces_box2d_world_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_box2d_world_component : public ces_base_component, public b2ContactListener
    {
    public:
        
        static const f32 k_box2d_world_scale;
        
    private:
        
        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
        
    protected:
        
        b2Vec2 m_gravity;
        std::shared_ptr<b2World> m_box2d_world;
        b2Body* m_box2d_body;
        b2BodyDef m_box2d_body_definition;
        
    public:
        
        CTTI_CLASS_GUID(ces_box2d_world_component, ces_base_component::g_guids_container)
        ces_box2d_world_component();
        ~ces_box2d_world_component();
        
        std::property_ro<std::shared_ptr<b2World>> box2d_world;
        
        void set_world_bounds(const glm::vec2& min_bound, const glm::vec2& max_bound);
    };
};

#endif
