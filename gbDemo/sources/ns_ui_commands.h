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

namespace ns
{
    namespace on_goto_server_menu
    {
        typedef std::function<void()> t_command;
        const std::string guid = "on_goto_server_menu";
    };
    
    namespace on_goto_client_menu
    {
        typedef std::function<void()> t_command;
        const std::string guid = "on_goto_client_menu";
    };
};

