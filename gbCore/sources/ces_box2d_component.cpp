//
//  ces_box2d_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_component.h"

namespace gb
{
    ces_box2d_component::ces_box2d_component() :
    m_box2d_body(nullptr),
    m_box2d_body_definition(std::make_shared<b2BodyDef>()),
    m_position(glm::vec3(0.f)),
    m_rotation(0.f)
    {
        m_type = e_ces_component_type_box2d;
    }
    
    ces_box2d_component::~ces_box2d_component()
    {
        
    }
    
    void ces_box2d_component::on_position_changed(const glm::vec3& position)
    {
        m_position = position;
    }
    
    void ces_box2d_component::on_rotation_changed(f32 rotation)
    {
        m_rotation = rotation;
    }
    
    glm::vec3 ces_box2d_component::get_position() const
    {
        return m_position;
    }
    
    f32 ces_box2d_component::get_rotation() const
    {
        return m_rotation;
    }
    
    std::shared_ptr<b2BodyDef> ces_box2d_component::get_box2d_body_definition() const
    {
        return m_box2d_body_definition;
    }
    
    void ces_box2d_component::set_box2d_body(b2Body* box2d_body)
    {
        m_box2d_body = box2d_body;
    }
    
    b2Body* ces_box2d_component::get_box2d_body() const
    {
        return m_box2d_body;
    }
}