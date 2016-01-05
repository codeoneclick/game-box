//
//  game_controller.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_controller.h"
#include "game_transition.h"
#include "game_loop.h"
#include "configuration_types.h"
#include "graphics_context.h"
#include "input_context.h"
#include "configuration_accessor.h"
#include "resource_accessor.h"

namespace gb
{
    game_controller::game_controller(const std::shared_ptr<ogl_window>& window) :
    m_current_transition(nullptr)
    {
#if defined (__IOS__)
        
        m_graphics_context = graphics_context::construct(window, e_graphic_context_api_ios);
        m_input_context = input_context::construct(window, e_input_context_api_ios);
        
#elif defined(__OSX__)
        
        m_graphics_context = graphics_context::construct(window, e_graphic_context_api_osx);
        m_input_context = input_context::construct(window, e_input_context_api_osx);
        
#endif

        m_configuration_accessor = std::make_shared<configuration_accessor>();
        m_resource_accessor = std::make_shared<resource_accessor>();
        
        add_listener_to_game_loop(m_resource_accessor);
    }
    
    game_controller::~game_controller(void)
    {
        m_transitions.clear();
    }
    
    void game_controller::add_transition(const std::shared_ptr<game_transition>& transition)
    {
        m_transitions.insert(std::make_pair(transition->get_guid(), transition));
    }
    
    void game_controller::remove_transition(const std::shared_ptr<game_transition>& transition)
    {
        assert(m_transitions.find(transition->get_guid()) != m_transitions.end());
        m_transitions.erase(m_transitions.find(transition->get_guid()));
    }
    
    void game_controller::goto_transition(const std::string& guid)
    {
        assert(m_transitions.find(guid) != m_transitions.end());
        if(m_current_transition != nullptr)
        {
            m_current_transition->on_deactivated();
            m_input_context->remove_listener(m_current_transition);
            remove_listener_from_game_loop(m_current_transition);
        }
        m_current_transition = m_transitions.find(guid)->second;
        m_current_transition->on_activated(m_graphics_context,
                                           m_input_context,
                                           m_configuration_accessor,
                                           m_resource_accessor);
        
        m_input_context->add_listener(m_current_transition);
        add_listener_to_game_loop(m_current_transition);
    }
}