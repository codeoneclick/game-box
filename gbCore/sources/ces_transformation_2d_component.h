//
//  ces_transformation_2d_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_transformation_2d_component : public ces_base_component
    {
    public:
        
        static const f32 k_z_order_step;
        
    private:
        
    protected:
        
        glm::vec2 m_position;
        f32 m_rotation;
        glm::vec2 m_scale;
        f32 m_z_order;
        
        glm::mat4 m_matrix_t;
        glm::mat4 m_matrix_r;
        glm::mat4 m_matrix_s;
        
        glm::mat4 m_matrix_m;
        
        glm::mat4 m_absolute_matrix_m;
        
        bool m_is_matrix_m_computed;
        bool m_is_in_camera_space;
        
        ui32 m_matrix_m_version;
        ui32 m_absolute_matrix_version;
        
    public:
        
        CTTI_CLASS_GUID(ces_transformation_2d_component, ces_base_component::g_guids_container)
        ces_transformation_2d_component();
        ~ces_transformation_2d_component();
        
        ces_transformation_2d_component(const ces_transformation_2d_component& copy) = delete;
        ces_transformation_2d_component& operator=(const ces_transformation_2d_component& copy) = delete;
        
        ui32 get_matrix_m_version() const;
        ui32 get_absolute_matrix_version() const;
    
        void set_position(const glm::vec2& position);
        void set_rotation(f32 rotation);
        void set_scale(const glm::vec2& scale);
        void set_z_order(f32 z_order);
        
        glm::vec2 get_position() const;
        f32 get_rotation() const;
        glm::vec2 get_scale() const;
        f32 get_z_order() const;
        
        glm::mat4 get_matrix_m();
        void set_custom_matrix_m(const glm::mat4& matrix_m);
        
        void update_absolute_transformation(const glm::mat4& parent_mat_m);
        glm::mat4 get_absolute_transformation();
        
        void set_is_in_camera_space(bool value);
        bool is_in_camera_space() const;
    };
};
