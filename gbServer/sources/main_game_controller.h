//
//  main_game_controller.h
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_controller.h"
#include "ns_declarations.h"

namespace ns
{
    class main_game_controller : public gb::game_controller
    {
    private:
        
    protected:
        
         in_game_transition_shared_ptr m_game_transition;
        
    public:
        
        main_game_controller(const std::shared_ptr<gb::ogl_window>& window);
        ~main_game_controller();
    };
};
