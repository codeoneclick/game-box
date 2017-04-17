//
//  input_context.h
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
#if defined(__WIN32__)

	extern LRESULT CALLBACK win32_callback(HWND hwnd, UINT message, WPARAM param_w, LPARAM param_l);

#endif

    enum e_input_context_api
    {
        e_input_context_api_osx = 1,
        e_input_context_api_ios,
		e_input_context_api_win32,
        e_input_context_api_tvos
    };
    
    enum e_input_source
    {
        e_input_source_none = 0,
        e_input_source_mouse_left,
        e_input_source_mouse_right,
        e_input_source_keyboard,
        e_input_source_max
    };
    
    enum e_input_state
    {
        e_input_state_pressed = 0,
        e_input_state_released,
        e_input_state_moved,
        e_input_state_dragged,
        e_input_state_max
    };

    class input_context_listener
    {
    private:
        
    protected:
        
        input_context_listener() = default;
        
    public:
        
        virtual ~input_context_listener() = default;
        
        virtual void on_gr_pressed(const glm::ivec2& point, e_input_source input_source,
                                   ui32 index) = 0;
        virtual void on_gr_released(const glm::ivec2& point, e_input_source input_source,
                                    ui32 index) = 0;
        virtual void on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta,
                                 ui32 index) = 0;
        virtual void on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta,
                                   e_input_source e_input_source, ui32 index) = 0;
        
        virtual void on_key_up(i32 key) = 0;
        virtual void on_key_down(i32 key) = 0;
        
        virtual void on_virtual_keyboard_input(const std::string& symbol) = 0;
        virtual void on_virtual_keyboard_backspace() = 0;
    };
    
    class input_context
    {
    public:
        
        typedef std::function<void()> show_virtual_keyboard_callback_t;
        typedef std::function<void()> hide_virtual_keyboard_callback_t;
        
    private:
        
        glm::ivec2 m_previous_touch_point;
        
    protected:
        
        std::set<std::shared_ptr<input_context_listener>> m_listeners;
        input_context();
        
        show_virtual_keyboard_callback_t m_show_virtual_keyboard_callback;
        hide_virtual_keyboard_callback_t m_hide_virtual_keyboard_callback;
        
    public:
        
        virtual ~input_context();
        
        glm::ivec2 get_previous_touch_point() const;
        void set_previous_touch_point(const glm::ivec2 point);
        
        static std::shared_ptr<input_context> construct(const std::shared_ptr<ogl_window>& window, e_input_context_api api);
        
        void gr_pressed(const glm::ivec2& point, e_input_source input_source,
                        ui32 index);
        void gr_released(const glm::ivec2& point, e_input_source input_source,
                         ui32 index);
        void gr_moved(const glm::ivec2& point, const glm::ivec2& delta,
                      ui32 index);
        void gr_dragged(const glm::ivec2& point, const glm::ivec2& delta,
                        e_input_source input_source, ui32 index);
        
        void key_up(i32 key);
        void key_down(i32 key);
        
        void virtual_keyboard_input(const std::string& symbol);
        void virtual_keyboard_backspace();

        
        virtual void show_virtual_keyboard();
        virtual void hide_virtual_keyboard();
        
        show_virtual_keyboard_callback_t get_show_virtual_keyboard_callback() const;
        hide_virtual_keyboard_callback_t get_hide_virtual_keyboard_callback() const;
        
        void add_listener(const input_context_listener_shared_ptr& listener);
        void remove_listener(const input_context_listener_shared_ptr& listener);
    };
};

#endif
