//
//  game_controller.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class game_transition;
    
    class game_controller
    {
    private:
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        graphics_context_shared_ptr m_graphics_context;
        input_context_shared_ptr m_input_context;

#endif
        configuration_accessor_shared_ptr m_configuration_accessor;
        resource_accessor_shared_ptr m_resource_accessor;
        
    protected:
        
        std::map<std::string, std::shared_ptr<game_transition>> m_transitions;
        std::shared_ptr<game_transition> m_current_transition;
        
    public:
        
        game_controller(const std::shared_ptr<window_impl>& window);
        virtual ~game_controller();
        
        void add_transition(const std::shared_ptr<game_transition>& transition);
        void remove_transition(const std::shared_ptr<game_transition>& transition);
        void goto_transition(const std::string& guid);
    };
};
