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
            
            bool m_current_value;
            
            void on_switch(const ces_entity_shared_ptr& entity);
            void on_switching(const ces_entity_shared_ptr& entity, f32 deltatime);
            
        public:
            
            switcher(const scene_fabricator_shared_ptr& fabricator);
            ~switcher();
            
            static switcher_shared_ptr construct(const scene_fabricator_shared_ptr& fabricator);
            
            void create();
        
            void set_on_switch_callback(const t_on_switch_callback& callback);
            
            void switch_value(bool value);
        };
    };
};

#endif
