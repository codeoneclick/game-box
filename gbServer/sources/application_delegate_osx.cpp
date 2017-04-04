//
//  application_delegate_osx.mm
//  gbDemo
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "application_delegate_osx.h"
#include "game_conroller_osx.h"

namespace game
{
    application_delegate_osx::application_delegate_osx()
    {
        m_game_controller = std::make_shared<game_conroller_osx>();
    }
    
    application_delegate_osx::~application_delegate_osx()
    {
        
    }
}
