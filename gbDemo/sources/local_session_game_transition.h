//
//  local_session_game_transition.h
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "game_transition.h"
#include "ns_declarations.h"

namespace game
{
    class local_session_game_transition : public gb::game_transition
    {
    private:
        
    protected:
        
        void create_scene();
        void destroy_scene();
        
    public:
        
		local_session_game_transition(const std::string& guid, bool is_offscreen);
        ~local_session_game_transition();
    };
};
