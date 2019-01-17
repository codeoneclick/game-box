//
//  joystick.h
//  gbUI
//
//  Created by serhii serhiiv on 10/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef joystick_h
#define joystick_h

#include "interaction_control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class joystick : public interaction_control
        {
        private:
            
        protected:
            
            virtual void on_touched(const ces_entity_shared_ptr&,
                                    const glm::vec2& touch_point,
                                    e_input_source input_source,
                                    e_input_state input_state) override;
            
            virtual void on_dragging(const ces_entity_shared_ptr&,
                                     const glm::vec2& touch_point,
                                     e_input_source input_source,
                                     e_input_state input_state) override;

        public:
            
            joystick(const scene_fabricator_shared_ptr& fabricator);
            ~joystick() = default;
            
            virtual void setup_components() override;
            
            virtual void create() override;

            bool is_drag_started_callback_exist() const;
            bool is_dragging_callback_exist() const;
            bool is_drag_ended_callback_exist() const;
        };
    };
};

#endif
