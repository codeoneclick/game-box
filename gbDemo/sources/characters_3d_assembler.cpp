//
//  characters_3d_assembler.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 5/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "characters_3d_assembler.h"
#include "camera_3d.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "sprite.h"

namespace game
{
    const f32 characters_3d_assembler::k_viewport_width = 16;
    const f32 characters_3d_assembler::k_viewport_height = 16;
    
    const i32 characters_3d_assembler::k_slots_cols = 4;
    const i32 characters_3d_assembler::k_slots_rows = 4;
    
    
    characters_3d_assembler::characters_3d_assembler()
    {
        m_used_slots_cols.fill(0);
        m_used_slots_rows.fill(0);
    }
    
    characters_3d_assembler::~characters_3d_assembler()
    {
        
    }
    
    void characters_3d_assembler::assemble(const gb::sprite_shared_ptr& entity_2d,
                                           const gb::shape_3d_shared_ptr& entity_3d,
                                           const glm::vec2& size,
                                           e_view_type view_type)
    {
        f32 scale = .02f;
        
        f32 position_offset_x = (k_viewport_width / static_cast<f32>(k_slots_cols));
        f32 position_offset_y = (k_viewport_width / static_cast<f32>(k_slots_rows));
        
        f32 offset_to_center_x = position_offset_x * .5f;
        f32 offset_to_center_y = position_offset_y * .5f;
        
        position_offset_x = m_used_slots_rows[view_type] * position_offset_x + offset_to_center_x;
        position_offset_y = m_used_slots_cols[view_type] * position_offset_y + offset_to_center_y;
        
        f32 texcoord_step_x = 1.f / static_cast<f32>(k_slots_cols);
        f32 texcoord_step_y = 1.f / static_cast<f32>(k_slots_rows);
        
        f32 texcoord_offset_x = texcoord_step_x * m_used_slots_cols[view_type];
        f32 texcoord_offset_y = 1.f - texcoord_step_y * m_used_slots_rows[view_type];
        
        m_used_slots_cols[view_type]++;
        if(m_used_slots_cols[view_type] >= k_slots_cols)
        {
            m_used_slots_cols[view_type] = 0;
            m_used_slots_rows[view_type]++;
            if(m_used_slots_rows[view_type] >= k_slots_rows)
            {
                assert(false);
            }
        }
        
        entity_3d->position = glm::vec3(0.f, position_offset_x, position_offset_y);
        entity_3d->scale = glm::vec3(scale);
        entity_3d->play_animation("idle");
        entity_3d->rotation = glm::vec3(0.f, -90.f, view_type == e_view_type_top ? 90.f : 0.f);
        
        entity_2d->size = size;
        entity_2d->pivot = glm::vec2(.5f);
        entity_2d->set_custom_texcoord(glm::vec4(texcoord_offset_x,
                                              texcoord_offset_y - texcoord_step_y,
                                              texcoord_offset_x + texcoord_step_x,
                                              texcoord_offset_y));
        entity_2d->rotation = view_type == e_view_type_top ? 180.f : 0.f;
        entity_2d->tag = "body";
    }
}
