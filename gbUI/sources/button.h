//
//  button.h
//  gbUI
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef button_h
#define button_h

#include "control.h"

namespace gb
{
    namespace ui
    {
        class button : public control
        {
        private:
            
        protected:
            
        public:
            
            button(const scene_fabricator_shared_ptr& fabricator);
            ~button();
            
            void create();
            
            void set_size(const glm::vec2& size);
            glm::vec2 get_size() const;
            
            void set_text(const std::string& text);
            std::string get_text();
        };
    };
};

#endif
