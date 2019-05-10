//
//  ces_scene_fabricator_component.cpp
//  gbDemo
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_scene_fabricator_component.h"

namespace game
{
    ces_scene_fabricator_component::ces_scene_fabricator_component()
    {
        gameplay_fabricator.getter([=]() {
            gameplay_fabricator_shared_ptr result = nullptr;
            if (!m_gameplay_fabricator.expired())
            {
                result = m_gameplay_fabricator.lock();
            }
            return result;
        });
        
        gameplay_ui_fabricator.getter([=]() {
            gameplay_ui_fabricator_shared_ptr result = nullptr;
            if (!m_gameplay_ui_fabricator.expired())
            {
                result = m_gameplay_ui_fabricator.lock();
            }
            return result;
        });
    }
    
    void ces_scene_fabricator_component::set_parameters(const gameplay_fabricator_shared_ptr& gameplay_fabricator,
                                                        const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator)
    {
        m_gameplay_fabricator = gameplay_fabricator;
        m_gameplay_ui_fabricator = gameplay_ui_fabricator;
    }
}
