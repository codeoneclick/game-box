//
//  ces_box2d_component.h
//  gbCore
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_box2d_component_h
#define ces_box2d_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_box2d_component : public ces_base_component
    {
    private:
        
    protected:
        
        b2Body* m_box2d_body;
        std::shared_ptr<b2BodyDef> m_box2d_body_definition;
        
        glm::vec3 m_position;
        f32 m_rotation;
        
    public:
        
        ces_box2d_component();
        ~ces_box2d_component();
        
        void on_position_changed(const glm::vec3& position);
        void on_rotation_changed(f32 rotation);
        
        glm::vec3 get_position() const;
        f32 get_rotation() const;
        
        std::shared_ptr<b2BodyDef> get_box2d_body_definition() const;
        
        void set_box2d_body(b2Body* box2d_body);
        b2Body* get_box2d_body() const;
    };
};

#endif
