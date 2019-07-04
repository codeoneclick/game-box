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
#include "interaction_control.h"

namespace gb
{
    namespace ui
    {
        class dialog : public interaction_control
        {
        private:
            
            glm::vec4 m_bounds = glm::vec4(std::numeric_limits<f32>::max(),
                                           std::numeric_limits<f32>::max(),
                                           std::numeric_limits<f32>::min(),
                                           std::numeric_limits<f32>::min());
            
        protected:
            
            std::map<i32, ces_entity_shared_ptr> m_controls;
            glm::u8vec4 m_label_color;
            
        public:
            
            dialog(const scene_fabricator_shared_ptr& fabricator);
            ~dialog();
            
            virtual void create() override;

            const std::map<i32, ces_entity_shared_ptr>& get_controls() const;
            
            void add_control(const ces_entity_shared_ptr& control, i32 id);
            void remove_control(i32 id);
            ces_entity_shared_ptr get_control(i32 id) const;
            
            void show_title(bool value);
            void set_title(const std::string& title);
            void set_title_color(const glm::u8vec4& color);
            void set_title_text_color(const glm::u8vec4& color);
        };
    };
};
