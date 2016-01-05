//
//  scene_graph.h
//  gbCore
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef scene_graph_h
#define scene_graph_h

#include "ces_entity.h"

namespace gb
{
    class scene_graph : public ces_entity
    {
    private:
        
        std::weak_ptr<game_transition> m_transition;
        scene_fabricator_shared_ptr m_fabricator;
        
    protected:
        
        game_transition_shared_ptr get_transition() const;
        scene_fabricator_shared_ptr get_fabricator() const;
        
        game_commands_container_shared_ptr m_internal_commands;
        game_commands_container_shared_ptr m_external_commands;
    
        
    public:
        
        scene_graph(const game_transition_shared_ptr& transition);
        ~scene_graph();
        
        void set_fabricator(const scene_fabricator_shared_ptr& fabricator);
        
        void set_external_commands(const game_commands_container_shared_ptr& commands);
        game_commands_container_shared_ptr get_internal_commands() const;
    };
};

#endif
