//
//  ces_geometry_quad_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_geometry_component.h"

namespace gb
{
    class ces_geometry_quad_component : public ces_geometry_component
    {
    private:
        
    protected:

        glm::vec2 m_pivot;
        glm::vec2 m_size;
        glm::vec4 m_texcoord;
        
        void update_mesh_position_attributes();
        void update_mesh_texcoord_attributes();
        
    public:
        
        ces_geometry_quad_component(bool is_use_batch = false);
        ~ces_geometry_quad_component();
        
        void set_size(const glm::vec2& size);
        glm::vec2 get_size() const;
        
        void set_pivot(const glm::vec2& pivot);
        glm::vec2 get_pivot() const;
        
        void update_texcoord(const glm::vec4& texcoord);
    };
};
