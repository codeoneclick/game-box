//
//  ces_shadow_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_shadow_component.h"
#include "glm_extensions.h"

namespace gb
{
    ces_shadow_component::ces_shadow_component()
    {
        m_type = e_ces_component_type_shadow;
    }
    
    ces_shadow_component::~ces_shadow_component()
    {
        
    }
    
    void ces_shadow_component::update_shadow_geometry(const glm::vec2& light_caster_position, const glm::mat4& shadow_caster_mat_m,
                                                      const vbo::vertex_attribute* shadow_caster_vertices, i32 shadow_caster_vertices_count)
    {
        std::vector<bool> back_facing;
        back_facing.resize(shadow_caster_vertices_count, false);
        for(i32 i = 0; i < shadow_caster_vertices_count; ++i)
        {
            glm::vec2 edge_01 = glm::transform(shadow_caster_vertices[i].m_position, shadow_caster_mat_m);
            i32 point_02_index = (i + 1) % shadow_caster_vertices_count;
            
            glm::vec2 edge_02 =  glm::transform(shadow_caster_vertices[point_02_index].m_position, shadow_caster_mat_m);
            glm::vec2 middle = (edge_01 + edge_02) / 2.f;
            
            glm::vec2 light_direction = light_caster_position - middle;
            glm::vec2 normal = glm::vec2(edge_01.y - edge_02.y,
                                         edge_01.x - edge_02.x);
            
            if (glm::dot(light_direction, normal) > 0.f)
            {
                back_facing[i] = false;
            }
            else
            {
                back_facing[i] = true;
            }
        }
        
        int startingIndex = 0;
        int endingIndex = 0;
        for (int i = 0; i < shadow_caster_vertices_count; i++)
        {
            int currentEdge = i;
            int nextEdge = (i + 1) % shadow_caster_vertices_count;
            
            if (back_facing[currentEdge] && !back_facing[nextEdge])
                endingIndex = nextEdge;
            
            if (!back_facing[currentEdge] && back_facing[nextEdge])
                startingIndex = nextEdge;
        }
        
        int shadowVertexCount;
        
        //nr of vertices that are in the shadow
        
        if (endingIndex > startingIndex)
            shadowVertexCount = endingIndex - startingIndex+1;
        else
            shadowVertexCount = shadow_caster_vertices_count + 1 - startingIndex + endingIndex ;

        std::cout<<std::endl;
    }
    
    void ces_shadow_component::generate_mesh()
    {
        
    }
    
    void ces_shadow_component::cleanup()
    {
        
    }
};