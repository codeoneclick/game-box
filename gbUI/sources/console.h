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
            f32 m_line_height;
            std::list<ces_entity_weak_ptr> m_messages;
            
        public:
            
            console(const scene_fabricator_shared_ptr& fabricator);
            ~console();
            
            static console_shared_ptr construct(const scene_fabricator_shared_ptr& fabricator);
            
            void create();
            
            void set_lines_count(i32 lines_count);
            
            void write(const std::string& message);
            
            void clear();
        };
    };
};

