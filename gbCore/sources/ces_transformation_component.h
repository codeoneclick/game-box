//
//  ces_transformation_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_transformation_component_h
#define ces_transformation_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_transformation_component : public ces_base_component
    {
    private:
        
    protected:
        
        glm::vec2 m_position;
        f32 m_rotation;
        glm::vec2 m_scale;
        
        glm::mat4 m_matrix_t;
        glm::mat4 m_matrix_r;
        glm::mat4 m_matrix_s;
        
        glm::mat4 m_matrix_m;
        
        bool m_is_matrix_m_computed;
        
        bool m_is_in_camera_space;
        
    public:
        
        CTTI_GUID_H(ces_transformation_component)
        ces_transformation_component();
        ~ces_transformation_component();
    
        void set_position(const glm::vec2& position);
        void set_rotation(f32 rotation);
        void set_scale(const glm::vec2& scale);
        
        glm::vec2 get_position() const;
        f32 get_rotation() const;
        glm::vec2 get_scale() const;
        
        glm::mat4 get_matrix_m();
        
        glm::mat4 add_parent_transformation(const glm::mat4& child_mat_m) const;
        
        void set_is_in_camera_space(bool value);
        bool is_in_camera_space() const;
    };
    
#define unsafe_get_transformation_component(entity) static_cast<ces_transformation_component*>(entity->get_component(ces_transformation_component::class_guid()).get())
#define unsafe_get_transformation_component_from_this static_cast<ces_transformation_component*>(ces_entity::get_component(ces_transformation_component::class_guid()).get())
    
};


#endif
