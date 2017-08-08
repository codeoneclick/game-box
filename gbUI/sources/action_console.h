//
//  action_console.h
//  gbUI
//
//  Created by serhii serhiiv on 3/31/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"

namespace gb
{
    namespace ui
    {
        class action_console : public control
        {
        private:
            
            f32 m_visible_timeinterval;
            
        protected:
            
            i32 m_lines_max_count;
            f32 m_line_height;
            std::list<ces_entity_weak_ptr> m_unused_messages;
            std::list<std::tuple<ces_entity_weak_ptr, f32>> m_messages;
            
            void on_message_action(const gb::ces_entity_shared_ptr& entity, f32 dt);
            void update_lines_position();
            
        public:
            
            action_console(const scene_fabricator_shared_ptr& fabricator);
            ~action_console();
            
            void create() override;
            
            void set_lines_count(i32 lines_count);
            
            void write(const std::string& message);
            
            void clear();
        };
    };
};
