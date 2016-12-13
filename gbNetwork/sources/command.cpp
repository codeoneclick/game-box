//
//  command.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "command.h"

namespace gb
{
    namespace net
    {
        const i32 command::k_header_size = 8;
        
        command::command() :
        m_id(-1)
        {
            
        }
        
        command::~command()
        {
            
        }
    }
}
