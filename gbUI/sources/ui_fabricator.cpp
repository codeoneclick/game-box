//
//  ui_fabricator.cpp
//  gbUI
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ui_fabricator.h"
#include "button.h"
#include "grouped_buttons.h"
#include "table_view.h"
#include "tree_view.h"
#include "content_tab_list.h"
#include "switcher.h"

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
            button_shared_ptr button = std::make_shared<gb::ui::button>(m_fabricator);
            
            button->create();
            button->set_size(size);
            
            m_controls_container.insert(button);
            
            return button;
        }
        
        grouped_buttons_shared_ptr ui_fabricator::create_grouped_buttons(const glm::vec2& size,
                                                                         std::function<void(i32, const ces_entity_shared_ptr&)> callback)
        {
            grouped_buttons_shared_ptr grouped_buttons = std::make_shared<gb::ui::grouped_buttons>(m_fabricator);
            
            grouped_buttons->create();
            grouped_buttons->set_size(size);
            grouped_buttons->set_on_pressed_callback(callback);
            
            m_controls_container.insert(grouped_buttons);
            
            return grouped_buttons;
        }
        
        table_view_shared_ptr ui_fabricator::create_table_view(const glm::vec2& size)
        {
            table_view_shared_ptr table_view = std::make_shared<gb::ui::table_view>(m_fabricator);
            
            table_view->create();
            table_view->set_size(size);
            
            m_controls_container.insert(table_view);
            
            return table_view;
        }
        
        tree_view_shared_ptr ui_fabricator::create_tree_view(const glm::vec2& size)
        {
            tree_view_shared_ptr tree_view = std::make_shared<gb::ui::tree_view>(m_fabricator);
            
            tree_view->create();
            tree_view->size = size;
            
            m_controls_container.insert(tree_view);
            
            return tree_view;
        }
        
        content_tab_list_shared_ptr ui_fabricator::create_content_tab_list(const glm::vec2& size)
        {
            content_tab_list_shared_ptr content_tab_list = std::make_shared<gb::ui::content_tab_list>(m_fabricator);
            
            content_tab_list->create();
            content_tab_list->set_size(size);
            
            m_controls_container.insert(content_tab_list);
            
            return content_tab_list;
        }
        
        switcher_shared_ptr ui_fabricator::create_switcher(const glm::vec2& size)
        {
            switcher_shared_ptr switcher = std::make_shared<gb::ui::switcher>(m_fabricator);
            
            switcher->create();
            switcher->set_size(size);
            
            m_controls_container.insert(switcher);
            return switcher;
        }
    }
}