//
//  ces_transformation_3d_component.h
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_transformation_component.h"

namespace gb
{
    class ces_transformation_3d_component : public ces_transformation_component
    {
    private:
        
    protected:
        
        glm::vec3   m_position;
        glm::vec3   m_rotation;
        glm::vec3   m_scale;
        
    public:
        
        ces_transformation_3d_component();
        ~ces_transformation_3d_component();
        
        ces_transformation_3d_component(const ces_transformation_3d_component& copy) = delete;
        ces_transformation_3d_component& operator=(const ces_transformation_3d_component& copy) = delete;
        
        void set_position(const glm::vec3& position);
        void set_rotation(const glm::vec3& rotation);
        void set_scale(const glm::vec3& scale);
        
        glm::vec3 get_position() const;
        glm::vec3 get_rotation() const;
        glm::vec3 get_scale() const;
        glm::vec3 get_absolute_position();
    };
};
