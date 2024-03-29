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
#include "window_impl.h"
#include "configuration_types.h"
#include "graphics_context.h"
#include "input_context.h"
#include "configuration_accessor.h"
#include "resource_accessor.h"

namespace gb
{
    game_controller::game_controller(const std::shared_ptr<window_impl>& window) :
    m_window(window),
    m_current_transition(nullptr)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        assign_hwnd_to_game_loop(window->get_hwnd());

	#if defined (__IOS__)
        
        m_graphics_context = graphics_context::construct(window, e_graphic_context_api_ios);
        m_input_context = input_context::construct(window, e_input_context_api_ios);
        
	#elif defined(__OSX__)
        
        m_graphics_context = graphics_context::construct(window, e_graphic_context_api_osx);
        m_input_context = input_context::construct(window, e_input_context_api_osx);

	#elif defined(__WINOS__)

		m_graphics_context = graphics_context::construct(window, e_graphic_context_api_win32);
		m_input_context = input_context::construct(window, e_input_context_api_win32);

    #elif defined(__TVOS__)
        
        m_graphics_context = graphics_context::construct(window, e_graphic_context_api_tvos);
        m_input_context = input_context::construct(window, e_input_context_api_tvos);
        
	#endif
        
#if USED_GRAPHICS_API != METAL_API

        m_graphics_context->make_current();
        
#endif

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
            game_controller::remove_transition(m_current_transition);
        }
        m_current_transition = m_transitions.find(guid)->second;

#if USED_GRAPHICS_API != NO_GRAPHICS_API

        m_current_transition->on_activated(m_graphics_context,
                                           m_input_context,
                                           m_configuration_accessor,
                                           m_resource_accessor);

#else

		m_current_transition->on_activated(nullptr,
										   nullptr,
										   m_configuration_accessor,
										   m_resource_accessor);

#endif
    }

    window_impl_shared_ptr game_controller::get_window() const
    {
        return m_window;
    }
}
