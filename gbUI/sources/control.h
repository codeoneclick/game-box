//
//  control.h
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef control_h
#define control_h

#include "main_headers.h"
#include "ui_declarations.h"

namespace gb
{
    namespace ui
    {
        class control
        {
        private:
            
        protected:
            
            glm::ivec2 m_position;
            glm::ivec2 m_size;
            
            std::vector<element_shared_ptr> m_elements;
            
        public:
            
            control();
            ~control();
            
            virtual void set_position(const glm::ivec2& position);
            glm::ivec2 get_position() const;
            
            virtual void set_size(const glm::ivec2& size);
            glm::ivec2 get_size() const;
            
            const std::vector<element_shared_ptr>& get_elements() const;
        };
    };
};


#endif
