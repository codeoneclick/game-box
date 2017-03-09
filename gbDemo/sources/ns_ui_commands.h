//
//  ns_ui_commands.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace game
{
    namespace on_goto_local_session
    {
        typedef std::function<void()> t_command;
        const std::string guid = "on_goto_local_session";
    };
    
    namespace on_goto_net_session
    {
        typedef std::function<void()> t_command;
        const std::string guid = "on_goto_net_session";
    };
};

