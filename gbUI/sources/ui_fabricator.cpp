//
//  ui_fabricator.cpp
//  gbUI
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ui_fabricator.h"
#include "button.h"
#include "image_button.h"
#include "panel.h"
#include "textfield.h"
#include "grouped_buttons.h"
#include "table_view.h"
#include "tree_view.h"
#include "content_tab_list.h"
#include "switcher.h"
#include "joystick.h"
#include "fullscreen_joystick.h"
#include "console.h"
#include "action_console.h"

namespace gb
{
    namespace ui
    {
        ui_fabricator::ui_fabricator(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator)
        {
            
        }
        
        ui_fabricator::~ui_fabricator()
        {
            
        }
        
        button_shared_ptr ui_fabricator::create_button(const glm::vec2& size, std::function<void(const ces_entity_shared_ptr&)> callback)
        {
            auto button = gb::ces_entity::construct<gb::ui::button>(m_fabricator);
            button->create();
            button->size = size;
            button->set_on_pressed_callback(callback);
            return button;
        }
        
        image_button_shared_ptr ui_fabricator::create_image_button(const glm::vec2& size, const std::string& image_filename, std::function<void(const ces_entity_shared_ptr&)> callback)
        {
            auto button = gb::ces_entity::construct<gb::ui::image_button>(m_fabricator);
            button->create(image_filename);
            button->size = size;
            button->set_on_pressed_callback(callback);
            return button;
        }

		panel_shared_ptr ui_fabricator::create_panel(const glm::vec2& size, const std::string& text)
		{
			auto panel = gb::ces_entity::construct<gb::ui::panel>(m_fabricator);
			panel->create();
			panel->size = size;
			panel->set_text(text);
			return panel;
		}
        
        grouped_buttons_shared_ptr ui_fabricator::create_grouped_buttons(const glm::vec2& size,
                                                                         std::function<void(i32, const ces_entity_shared_ptr&)> callback)
        {
            auto grouped_buttons = gb::ui::grouped_buttons::construct(m_fabricator);
            grouped_buttons->create();
            grouped_buttons->size = size;
            grouped_buttons->set_on_pressed_callback(callback);
            return grouped_buttons;
        }
        
        table_view_shared_ptr ui_fabricator::create_table_view(const glm::vec2& size)
        {
            auto table_view = gb::ces_entity::construct<gb::ui::table_view>(m_fabricator);
            table_view->create();
            table_view->size = size;
            return table_view;
        }
        
        tree_view_shared_ptr ui_fabricator::create_tree_view(const glm::vec2& size)
        {
            auto tree_view = gb::ui::tree_view::construct(m_fabricator);
            tree_view->create();
            tree_view->size = size;
            return tree_view;
        }
        
        content_tab_list_shared_ptr ui_fabricator::create_content_tab_list(const glm::vec2& size)
        {
            auto content_tab_list = gb::ui::content_tab_list::construct(m_fabricator);
            content_tab_list->create();
            content_tab_list->size = size;
            return content_tab_list;
        }
        
        switcher_shared_ptr ui_fabricator::create_switcher(const glm::vec2& size)
        {
            auto switcher = gb::ui::switcher::construct(m_fabricator);;
            switcher->create();
            switcher->size = size;
            return switcher;
        }
        
        joystick_shared_ptr ui_fabricator::create_joystick(const glm::vec2& size)
        {
            auto joystick = gb::ces_entity::construct<gb::ui::joystick>(m_fabricator);
            joystick->create();
            joystick->size = size;
            return joystick;
        }
        
        fullscreen_joystick_shared_ptr ui_fabricator::create_fullscreen_joystick(const glm::vec2& size, const glm::vec2& center)
        {
            auto fullscreen_joystick = gb::ui::fullscreen_joystick::construct(m_fabricator);;
            fullscreen_joystick->create();
            fullscreen_joystick->size = size;
            fullscreen_joystick->position = center;
            return fullscreen_joystick;
        }
        
        console_shared_ptr ui_fabricator::create_console(const glm::vec2& size, i32 lines_count)
        {
            auto console = gb::ui::console::construct(m_fabricator);
            console->create();
            console->size = size;
            console->set_lines_count(lines_count);
            return console;
        }
        
        action_console_shared_ptr ui_fabricator::create_action_console(const glm::vec2& size, i32 lines_count)
        {
            auto console = gb::ces_entity::construct<gb::ui::action_console>(m_fabricator);
            console->create();
            console->size = size;
            console->set_lines_count(lines_count);
            return console;
        }
        
        textfield_shared_ptr ui_fabricator::create_textfield(const glm::vec2& size, const std::string& text)
        {
            auto textfield = gb::ces_entity::construct<gb::ui::textfield>(m_fabricator);
            textfield->create();
            textfield->size = size;
            textfield->set_text(text);
            return textfield;
        }
    }
}
