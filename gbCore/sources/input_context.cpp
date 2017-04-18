//
//  input_context.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "input_context.h"

namespace gb
{
    extern std::shared_ptr<input_context> create_input_context_ios(const std::shared_ptr<ogl_window>& window);
    extern std::shared_ptr<input_context> create_input_context_osx(const std::shared_ptr<ogl_window>& window);
	extern std::shared_ptr<input_context> create_input_context_win32(const std::shared_ptr<ogl_window>& window);
    extern std::shared_ptr<input_context> create_input_context_tvos(const std::shared_ptr<ogl_window>& window);
    
    input_context::input_context() :
    m_show_virtual_keyboard_callback(nullptr),
    m_hide_virtual_keyboard_callback(nullptr)
    {
        
    }
    
    glm::ivec2 input_context::get_previous_touch_point() const
    {
        return m_previous_touch_point;
    }
    
    void input_context::set_previous_touch_point(const glm::ivec2 point)
    {
        m_previous_touch_point = point;
    }
    
    std::shared_ptr<input_context> input_context::construct(const std::shared_ptr<ogl_window>& window, e_input_context_api api)
    {
        std::shared_ptr<input_context> context = nullptr;
        switch (api)
        {
            case e_input_context_api_ios:
            {
#if defined(__IOS__)
                context = create_input_context_ios(window);
#else
                assert(false);
#endif
            }
                break;
                
            case e_input_context_api_osx:
            {
#if defined(__OSX__)
                context = create_input_context_osx(window);
#else
                assert(false);
#endif
            }
                break;

			case e_input_context_api_win32:
			{
#if defined(__WIN32__)
				context = create_input_context_win32(window);
#else
				assert(false);
#endif
			}
                break;
                
            case e_input_context_api_tvos:
            {
#if defined(__TVOS__)
                context = create_input_context_tvos(window);
#else
                assert(false);
#endif
            }
                break;

            default:
            {
                assert(false);
            }
            break;
        }
        assert(context != nullptr);
        return context;
    }
    
    input_context::~input_context(void)
    {
        m_listeners.clear();
    }
    
    void input_context::gr_pressed(const glm::ivec2 &point, gb::e_input_source input_source, ui32 index)
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_gr_pressed(point, input_source, index);
        }
    }
    
    void input_context::gr_released(const glm::ivec2 &point, e_input_source input_source, ui32 index)
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_gr_released(point, input_source, index);
        }
    }
    
    void input_context::gr_moved(const glm::ivec2& point, const glm::ivec2& delta, ui32 index)
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_gr_moved(point, delta, index);
        }
    }
    
    void input_context::gr_dragged(const glm::ivec2& point, const glm::ivec2& delta,
                                   e_input_source input_source, ui32 index)
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_gr_dragged(point, delta, input_source, index);
        }
    }
    
    void input_context::key_up(i32 key)
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_key_up(key);
        }
    }
    
    void input_context::key_down(i32 key)
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_key_down(key);
        }
    }
    
    void input_context::virtual_keyboard_input(const std::string& symbol)
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_virtual_keyboard_input(symbol);
        }
    }
    
    void input_context::virtual_keyboard_backspace()
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_virtual_keyboard_backspace();
        }
    }
    
    void input_context::virtual_keyboard_hidden()
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_virtual_keyboard_hidden();
        }
    }
    
    void input_context::add_listener(const input_context_listener_shared_ptr &listener)
    {
        m_listeners.insert(listener);
    }
    
    void input_context::remove_listener(const input_context_listener_shared_ptr &listener)
    {
        m_listeners.erase(listener);
    }
    
    void input_context::show_virtual_keyboard()
    {
        std::cout<<"unimplemented"<<std::endl;
    }
    
    void input_context::hide_virtual_keyboard()
    {
        std::cout<<"unimplemented"<<std::endl;
    }
    
    input_context::show_virtual_keyboard_callback_t input_context::get_show_virtual_keyboard_callback() const
    {
        return m_show_virtual_keyboard_callback;
    }
    
    input_context::hide_virtual_keyboard_callback_t input_context::get_hide_virtual_keyboard_callback() const
    {
        return m_hide_virtual_keyboard_callback;
    }
}

#endif
