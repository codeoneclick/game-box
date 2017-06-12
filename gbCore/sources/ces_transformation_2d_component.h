//
//  ces_transformation_2d_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_transformation_component.h"

namespace gb
{
    class ces_transformation_2d_component : public ces_transformation_component
    {
    public:
        
        static const f32 k_z_order_step;
        
    private:
        
    protected:
        
        glm::vec2 m_position;
        f32 m_rotation;
        glm::vec2 m_scale;
        
        f32 m_z_order;
        bool m_is_custom_z_order;
        
    public:
        
        ces_transformation_2d_component();
        ~ces_transformation_2d_component();
        
        ces_transformation_2d_component(const ces_transformation_2d_component& copy) = delete;
        ces_transformation_2d_component& operator=(const ces_transformation_2d_component& copy) = delete;
    
        void set_position(const glm::vec2& position);
        void set_rotation(f32 rotation);
        void set_scale(const glm::vec2& scale);
        void set_z_order(f32 z_order);
        void set_custom_z_order(f32 z_order);
        
        glm::vec2   get_position() const;
        f32         get_rotation() const;
        glm::vec2   get_scale() const;
        f32         get_z_order() const;
        bool        get_is_custom_z_order() const;
    };
};
