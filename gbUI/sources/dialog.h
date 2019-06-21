//
//  dialog.h
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ui_declarations.h"
#include "game_object_2d.h"

namespace gb
{
    namespace ui
    {
        class dialog : public game_object_2d
        {
        private:
            
        protected:
            
            std::map<i32, ces_entity_shared_ptr> m_controls;
            
        public:
            
            dialog();
            ~dialog();

            const std::map<i32, ces_entity_shared_ptr>& get_controls() const;
            
            void add_control(const ces_entity_shared_ptr& control, i32 id);
            void remove_control(i32 id);
            ces_entity_shared_ptr get_control(i32 id) const;
        };
    };
};
