//
//  application_delegate_osx.h
//  gbDemo
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "game_conroller_osx.h"

namespace ns
{
    class application_delegate_osx
    {
    private:
        
    protected:
        
        std::shared_ptr<game_conroller_osx> m_game_controller;
        
    public:
        application_delegate_osx();
        ~application_delegate_osx();
    };
};

