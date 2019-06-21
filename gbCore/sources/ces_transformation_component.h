//
//  ces_transformation_component.h
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_transformation_component : public ces_base_component
    {
    public:
        
        enum e_mode
        {
            e_mode_unknown = -1,
            e_mode_2d = 0,
            e_mode_3d
        };
        
    private:
        
    protected:
        
        glm::mat4 m_matrix_t;
        glm::mat4 m_matrix_r;
        glm::mat4 m_matrix_s;
        glm::mat4 m_matrix_m;
        
        glm::mat4 m_parent_matrix_m;
        glm::mat4 m_absolute_matrix_m;
        
        bool m_is_matrix_m_computed = false;
        bool m_is_absolute_matrix_m_computed = false;
        
        ui32 m_matrix_m_version;
        ui32 m_absolute_matrix_version;
        
        bool m_is_in_camera_space;
        e_mode m_mode;
        
    public:
        
        STTI_CLASS_GUID(ces_transformation_component, ces_base_component::g_guids_container)
        ces_transformation_component();
        ~ces_transformation_component();
        
        ces_transformation_component(const ces_transformation_component& copy) = delete;
        ces_transformation_component& operator=(const ces_transformation_component& copy) = delete;
        
        bool is_2d() const;
        bool is_3d() const;
        
        ces_transformation_2d_component_shared_ptr as_2d();
        ces_transformation_3d_component_shared_ptr as_3d();
        
        ui32 get_matrix_m_version() const;
        ui32 get_absolute_matrix_version() const;
        
        glm::mat4 get_matrix_m();
        
        virtual void update_absolute_transformation(const glm::mat4& parent_mat_m);
        virtual glm::mat4 get_absolute_transformation();
        
        void set_is_in_camera_space(bool value);
        bool is_in_camera_space() const;
    };
};

