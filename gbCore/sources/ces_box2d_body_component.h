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
    
    private:
        
    protected:
        
        glm::vec2 m_position;
        f32 m_rotation;
        
        b2Body* m_box2d_body;
        b2BodyDef* m_box2d_body_definition;
        
    public:
        
        CTTI_CLASS_GUID(ces_box2d_body_component, ces_base_component::g_guids_container)
        ces_box2d_body_component();
        ~ces_box2d_body_component();

        std::property_ro<glm::vec2> position;
        std::property_ro<f32> rotation;
        std::property_ro<b2BodyDef*> box2d_body_definition;
        
        std::property_rw<b2Body*> box2d_body;
        
        void on_position_changed(const glm::vec2& position);
        void on_rotation_changed(f32 rotation);
        
    };
};

#endif 
