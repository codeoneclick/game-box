//
//  ai_action.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ai_action_h
#define ai_action_h

#include "main_headers.h"
#include "declarations.h"
#include "koth_declarations.h"

namespace koth
{
    enum e_ai_action_state
    {
        e_ai_action_state_none = 0,
        e_ai_action_state_in_progress,
        e_ai_action_state_ended,
        e_ai_action_state_interapted
    };
    
    class ai_action : public std::enable_shared_from_this<ai_action>
    {
    private:
        
    protected:
        
        std::function<void(const ai_action_shared_ptr&)> m_start_callback;
        std::function<void(const ai_action_shared_ptr&)> m_in_progress_callback;
        std::function<void(const ai_action_shared_ptr&)> m_end_callback;
        
        e_ai_action_state m_state;
        
    public:
      
        ai_action();
        virtual ~ai_action();
        
        virtual void update(f32 deltatime) = 0;
 
        void set_start_callback(const std::function<void(const ai_action_shared_ptr&)>& start_callback);
        void set_in_progress_callback(const std::function<void(const ai_action_shared_ptr&)>& in_progress_callback);
        void set_end_callback(const std::function<void(const ai_action_shared_ptr&)>& end_callback);
        
        e_ai_action_state get_state() const;
        
        void interapt();
    };
};

#endif
