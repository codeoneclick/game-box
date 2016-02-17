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
#include "game_object.h"

namespace gb
{
    namespace ui
    {
        enum e_element_horizontal_aligment
        {
            e_element_horizontal_aligment_left = 0,
            e_element_horizontal_aligment_right,
            e_element_horizontal_aligment_center
        };
        
        enum e_element_vertical_aligment
        {
            e_element_vertical_aligment_top = 0,
            e_element_vertical_aligment_down,
            e_element_vertical_aligment_center
        };
        
        class control : public game_object
        {
        public:
            
            static glm::vec4 k_black_color;
            static glm::vec4 k_dark_gray_color;
            static glm::vec4 k_gray_color;
            static glm::vec4 k_light_gray_color;
            static glm::vec4 k_white_color;
            static glm::vec4 k_green_color;
            
        private:
            
            std::weak_ptr<scene_fabricator> m_fabricator;
            
        protected:
            
            glm::vec2 m_size;
            bool m_visible;
            
            std::map<std::string, game_object_shared_ptr> m_elements;
            
            scene_fabricator_shared_ptr get_fabricator() const;
            
            void set_element_horizontal_aligment(const game_object_shared_ptr& element,
                                                 e_element_horizontal_aligment aligment);
            
            void set_element_vertical_aligment(const game_object_shared_ptr& element,
                                               e_element_vertical_aligment aligment);
            
        public:
            
            control(const scene_fabricator_shared_ptr& fabricator);
            ~control();
            
            virtual void create() = 0;
            
            virtual void set_size(const glm::vec2& size);
            virtual glm::vec2 get_size() const;
        };
    };
};

#endif
