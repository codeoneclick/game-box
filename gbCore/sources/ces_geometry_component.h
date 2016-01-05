//
//  ces_geometry_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_geometry_component_h
#define ces_geometry_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_geometry_component : public ces_base_component
    {
    private:
        
    protected:

        mesh_shared_ptr m_mesh;
        
        glm::vec2 m_pivot;
        glm::vec4 m_frame;
        glm::vec4 m_texcoord;
        
        void update_mesh_position_attributes();
        void update_mesh_texcoord_attributes();
        
    public:
        
        ces_geometry_component();
        ~ces_geometry_component();
        
        void set_mesh(const mesh_shared_ptr& mesh);
        mesh_shared_ptr get_mesh() const;
        
        void set_size(const glm::vec2& size);
        glm::vec2 get_size() const;
        
        void set_pivot(const glm::vec2& pivot);
        glm::vec2 get_pivot() const;
    };
};

#endif
