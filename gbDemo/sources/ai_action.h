//
//  ai_action.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "std_extensions.h"

namespace game
{
    class ai_action : public std::enable_shared_from_this<ai_action>
    {
    public:
        
        enum e_ai_action_state
        {
            e_ai_action_state_none = 0,
            e_ai_action_state_in_progress,
            e_ai_action_state_ended,
            e_ai_action_state_interrupted
        };
        
    private:
        
    protected:
        
        static std::set<uintptr_t> g_guids_container;
        
        std::function<void(const ai_action_shared_ptr&)> m_start_callback;
        std::function<void(const ai_action_shared_ptr&)> m_in_progress_callback;
        std::function<void(const ai_action_shared_ptr&)> m_end_callback;
        
        e_ai_action_state m_state;
        
        std::queue<ai_action_shared_ptr> m_sub_actions;
        
    public:
      
        CTTI_CLASS_GUID(ai_action, ai_action::g_guids_container)
        ai_action();
        virtual ~ai_action();
        
        virtual void update(f32 deltatime) = 0;
        
        void add_sub_action(const ai_action_shared_ptr& action);
        std::queue<ai_action_shared_ptr>& get_sub_actions();
 
        void set_start_callback(const std::function<void(const ai_action_shared_ptr&)>& start_callback);
        void set_in_progress_callback(const std::function<void(const ai_action_shared_ptr&)>& in_progress_callback);
        void set_end_callback(const std::function<void(const ai_action_shared_ptr&)>& end_callback);
        
        bool is_start_callback_exist() const;
        bool is_progress_callback_exist() const;
        bool is_end_callback_exist() const;
        
        e_ai_action_state get_state() const;
        
        void interrupt();
    };
};
