//
//  net_declarations.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/9/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "declarations.h"

namespace gb
{
    namespace net
    {
        forward_decl(command)
        forward_decl(connection)
        forward_decl(ces_server_component)
        forward_decl(ces_client_component)
        forward_decl(ces_server_broadcast_component);
        forward_decl(ces_client_broadcast_component);
        forward_decl(ces_network_system)
        forward_decl(command_character_move)
        forward_decl(command_processor)
    };
};