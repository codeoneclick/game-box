//
//  ui_fabricator.cpp
//  gbUI
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ui_fabricator.h"
#include "button.h"
#include "content_list.h"
#include "content_tab_list.h"

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
        
        button_shared_ptr ui_fabricator::create_button(const glm::vec2& size, std::function<void(const button_shared_ptr&)> callback)
        {
            button_shared_ptr button = std::make_shared<gb::ui::button>(m_fabricator);
            
            button->create();
            button->set_size(size);
            
            m_controls_container.insert(button);
            
            return button;
        }
        
        content_list_shared_ptr ui_fabricator::create_content_list(const glm::vec2& size)
        {
            content_list_shared_ptr content_list = std::make_shared<gb::ui::content_list>(m_fabricator);
            
            content_list->create();
            content_list->set_size(size);
            
            m_controls_container.insert(content_list);
            
            return content_list;
        }
        
        content_tab_list_shared_ptr ui_fabricator::create_content_tab_list(const glm::vec2& size)
        {
            content_tab_list_shared_ptr content_tab_list = std::make_shared<gb::ui::content_tab_list>(m_fabricator);
            
            content_tab_list->create();
            content_tab_list->set_size(size);
            
            m_controls_container.insert(content_tab_list);
            
            return content_tab_list;
        }
    }
}