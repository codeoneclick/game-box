//
//  color_picker.hpp
//  gbUI
//
//  Created by serhii.s on 8/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "interaction_control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class color_picker : public interaction_control
        {
            public:
            
            static std::string k_color_wheel_element_name;
            
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
            
            color_picker(const scene_fabricator_shared_ptr& fabricator);
            virtual ~color_picker() = default;
            
            virtual void setup_components() override;
            
            void create() override;
        };
    };
};


