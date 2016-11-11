//
//  ces_box2d_body_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_body_component.h"

namespace gb
{
    ces_box2d_body_component::ces_box2d_body_component() :
    m_position(glm::vec2(0.f)),
    m_rotation(0.f),
    m_box2d_body_definition(new b2BodyDef()),
    m_box2d_body(nullptr)
    {
        position.getter([=]{
            return m_position;
        });
        
        rotation.getter([=] {
            return m_rotation;
        });
        
        box2d_body_definition.getter([=] {
            return m_box2d_body_definition;
        });
        
        box2d_body.getter([=] {
            return m_box2d_body;
        });
        
        box2d_body.setter([=] (b2Body* box2d_body) {
            m_box2d_body = box2d_body;
        });
    }
    
    ces_box2d_body_component::~ces_box2d_body_component()
    {
        
    }
    
    void ces_box2d_body_component::on_position_changed(const glm::vec2& position)
    {
        m_position = position;
    }
    
    void ces_box2d_body_component::on_rotation_changed(f32 rotation)
    {
        m_rotation = rotation;
    }
}
