//
//  demo_game_controller.h
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef demo_game_controller_h
#define demo_game_controller_h

#include "game_controller.h"

class demo_game_controller : public gb::game_controller
{
private:
    
protected:
    
public:
  
    demo_game_controller(const std::shared_ptr<gb::ogl_window>& window);
    ~demo_game_controller(void);
};

#endif
