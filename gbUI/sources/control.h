//
//  control.h
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ui_declarations.h"
#include "game_object_2d.h"

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
        
        class control : public game_object_2d
        {
        public:
            
            static glm::u8vec4 k_black_color;
            static glm::u8vec4 k_dark_gray_color;
            static glm::u8vec4 k_gray_color;
            static glm::u8vec4 k_light_gray_color;
            static glm::u8vec4 k_white_color;
            static glm::u8vec4 k_green_color;
            
            static std::string k_background_element_name;
            static std::string k_label_element_name;
            
        private:
            
        protected:
            
            std::weak_ptr<scene_fabricator> m_fabricator;
            
            glm::vec2 m_size;
            
            std::unordered_map<std::string, game_object_2d_shared_ptr> m_elements;
            std::unordered_map<std::string, std::string> m_sounds_linkage;
            
            scene_fabricator_shared_ptr get_fabricator() const;
            
            void set_element_horizontal_aligment(const game_object_2d_shared_ptr& element,
                                                 e_element_horizontal_aligment aligment,
												 const glm::vec2& custom_container_size = glm::vec2(-1.f));
            
            void set_element_vertical_aligment(const game_object_2d_shared_ptr& element,
                                               e_element_vertical_aligment aligment,
											   const glm::vec2& custom_container_size = glm::vec2(-1.f));
            
            void set_color(const std::string& element_name, const glm::u8vec4& color);
            
        public:
            
            control(const scene_fabricator_shared_ptr& fabricator);
            virtual ~control();

            virtual void setup_components() override;
            
            virtual void create();
            
            virtual void attach_sound(const std::string& filename, const std::string& state);
            
            virtual void set_background_color(const glm::u8vec4& color);
            virtual void set_alpha(ui8 alpha);
            
            template<typename T>
            std::shared_ptr<T> get_element_as(const std::string& element_name)
            {
                std::shared_ptr<T> element = nullptr;
                const auto element_it = m_elements.find(element_name);
                if (element_it != m_elements.end())
                {
                    element = std::static_pointer_cast<T>(element_it->second);
                }
                return element;
            };
        };
    };
};
