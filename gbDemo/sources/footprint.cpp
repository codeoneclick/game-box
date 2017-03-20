//
//  footprint.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "footprint.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"

namespace game
{
    footprint::footprint()
    {

    }
    
    footprint::~footprint()
    {
        
    }
    
    void footprint::setup(const std::string& filename,
                       const gb::scene_graph_shared_ptr& scene_graph,
                       const gb::scene_fabricator_shared_ptr& scene_fabricator,
                       const gb::anim::anim_fabricator_shared_ptr& anim_fabricator)
    {
        auto footprint = anim_fabricator->create_animated_sprite(filename, "footprint");
        footprint->tag = "footprint";
        footprint->goto_and_play(0);
        footprint->is_luminous = true;
        
        footprint::add_child(footprint);
    }
}
