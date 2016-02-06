//
//  switcher.h
//  gbUI
//
//  Created by Serhii Serhiiv on 2/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef switcher_hpp
#define switcher_hpp

#include "control.h"

namespace gb
{
    namespace ui
    {
        class switcher : public control
        {
        public:
            
            typedef std::function<void(bool, const ces_entity_shared_ptr&)> t_on_switch_callback;
            
        private:
            
        protected:
            
            t_on_switch_callback m_on_switch_callback;
            
            button_shared_ptr m_button;
            
            void on_switch(const ces_entity_shared_ptr& entity);
            
        public:
            
            switcher(const scene_fabricator_shared_ptr& fabricator);
            ~switcher();
            
            void create();
            
            void set_size(const glm::vec2& size);
            
            void set_on_switch_callback(const t_on_switch_callback& callback);
            
            void switch_value(bool value);
        };
    };
};

#endif
