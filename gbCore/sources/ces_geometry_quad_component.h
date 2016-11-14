//
//  ces_geometry_quad_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_geometry_quad_component_h
#define ces_geometry_quad_component_h

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
        
        ces_geometry_quad_component();
        ~ces_geometry_quad_component();
        
        void set_size(const glm::vec2& size);
        glm::vec2 get_size() const;
        
        void set_pivot(const glm::vec2& pivot);
        glm::vec2 get_pivot() const;
        
        void update_texcoord(const glm::vec4& texcoord);
    };
    
#define unsafe_get_geometry_quad_component(entity) static_cast<ces_geometry_quad_component*>(entity->get_component(ces_geometry_component::class_guid()).get())
#define unsafe_get_geometry_quad_component_from_this static_cast<ces_geometry_quad_component*>(ces_entity::get_component(ces_geometry_component::class_guid()).get())
    
};

#endif
