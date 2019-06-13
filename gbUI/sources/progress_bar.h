//
//  progress_bar.hpp
//  gbUI
//
//  Created by serhii.s on 6/12/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "interaction_control.h"
#include "ces_font_component.h"

namespace gb
{
    namespace ui
    {
        class progress_bar : public interaction_control
        {
        public:
            
            std::string k_foreground_element_name = "ui_forefround_element";
            std::string k_progress_line_element_name = "ui_progress_line_element";
            
        private:
            
            f32 m_progress = 0.f;
            
        protected:

            
        public:
            
            progress_bar(const scene_fabricator_shared_ptr& fabricator);
            ~progress_bar();
            
            void setup_components() override;
            
            void create() override;
            
            void set_progress(f32 progress);
            f32 get_progress() const;
            
            void set_progress_line_color(const glm::u8vec4& color);
            void set_foreground_color(const glm::u8vec4& color);
        };
    };
};

