//
//  i_scene_graph.h
//  gbCore
//
//  Created by sergey.sergeev on 10/5/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef i_scene_graph_h
#define i_scene_graph_h

#include "main_headers.h"
#include "declarations.h"
#include "game_loop.h"

namespace gb
{
    class i_scene_graph : public i_game_loop
    {
    private:
        
    protected:
        
        ces_systems_feeder_shared_ptr m_systems_feeder;
        
    public:
        
        i_scene_graph();
        ~i_scene_graph() = default;
        
        void set_systems_feeder(const ces_systems_feeder_shared_ptr& systems_feeder);
    };
};


#endif 
