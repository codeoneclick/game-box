//
//  console.h
//  gbUI
//
//  Created by serhii serhiiv on 12/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"

namespace gb
{
    namespace ui
    {
        class console : public control
        {
        private:
            
        protected:

            i32 m_lines_max_count;
            i32 m_lines_current_count;
            f32 m_line_height;
            
        public:
            
            console(const scene_fabricator_shared_ptr& fabricator);
            ~console();
            
            void create();
            
            void set_size(const glm::vec2& size);
            void set_lines_count(i32 lines_count);
            
            void write(const std::string& message);
            
            void clear();
        };
    };
};

