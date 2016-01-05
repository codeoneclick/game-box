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

namespace gb
{
    namespace ui
    {
        class dialog
        {
        private:
            
        protected:
            
            glm::ivec2 m_position;
            glm::ivec2 m_size;
            
            std::vector<element_shared_ptr> m_elements;
            std::map<i32, control_shared_ptr> m_controls;
            
        public:
            
            dialog();
            ~dialog();
            
            virtual void set_position(const glm::ivec2& position);
            glm::ivec2 get_position() const;
            
            virtual void set_size(const glm::ivec2& size);
            glm::ivec2 get_size() const;
            
            const std::vector<element_shared_ptr>& get_elements() const;
            const std::map<i32, control_shared_ptr>& get_controls() const;
            
            void add_control(const control_shared_ptr& control, i32 id);
            void remove_control(i32 id);
            control_shared_ptr get_control(i32 id) const;
        };
    };
};


#endif
