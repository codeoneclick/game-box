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
        
        void on_update(f32 deltatime);
        
    public:
        
        ces_transformation_component();
        ~ces_transformation_component();
    
        void set_position(const glm::vec2& position);
        void set_rotation(f32 rotation);
        void set_scale(const glm::vec2& scale);
        
        glm::vec2 get_position() const;
        f32 get_rotation() const;
        glm::vec2 get_scale() const;
        
        glm::mat4 get_matrix_m();
    };
};


#endif
