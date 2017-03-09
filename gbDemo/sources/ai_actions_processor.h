//
//  ai_actions_processor.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"

namespace game
{
    class ai_actions_processor
    {
    private:
        
    protected:
        
        std::queue<ai_action_shared_ptr> m_actions;
        
    public:
        
        ai_actions_processor();
        ~ai_actions_processor();
        
        void update(f32 deltatime);
        
        void add_action(const ai_action_shared_ptr& action);
        
        bool is_actions_exist() const;
        
        ai_action_shared_ptr top_action() const;
    };
}
