//
//  characters_3d_assembler.h
//  gbDemo
//
//  Created by serhii serhiiv on 5/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"

namespace game
{
    class characters_3d_assembler
    {
    public:
        
        enum e_view_type
        {
            e_view_type_top = 0,
            e_view_type_front,
            e_view_type_max
        };
        
    private:
        
        static const f32 k_viewport_width;
        static const f32 k_viewport_height;
        
        static const i32 k_slots_cols;
        static const i32 k_slots_rows;
        
        std::array<i32, e_view_type_max> m_used_slots_cols;
        std::array<i32, e_view_type_max> m_used_slots_rows;
        
    protected:
        
    public:
        
        characters_3d_assembler();
        ~characters_3d_assembler();
        
        void assemble(const gb::sprite_shared_ptr& entity_2d,
                      const gb::shape_3d_shared_ptr& entity_3d,
                      const glm::vec2& size,
                      e_view_type view_type);
    };
};
