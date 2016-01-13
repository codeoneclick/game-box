//
//  dialog.h
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef dialog_h
#define dialog_h

#include "main_headers.h"
#include "ui_declarations.h"
#include "sprite.h"

namespace gb
{
    namespace ui
    {
        class dialog : sprite
        {
        private:
            
        protected:
            
            glm::ivec2 m_position;
            glm::ivec2 m_size;
            
            std::vector<sprite_shared_ptr> m_elements;
            std::map<i32, control_shared_ptr> m_controls;
            
        public:
            
            dialog();
            ~dialog();
            
            const std::vector<sprite_shared_ptr>& get_elements() const;
            const std::map<i32, control_shared_ptr>& get_controls() const;
            
            void add_control(const control_shared_ptr& control, i32 id);
            void remove_control(i32 id);
            control_shared_ptr get_control(i32 id) const;
        };
    };
};


#endif
