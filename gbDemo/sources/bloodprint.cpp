//
//  bloodprint.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "bloodprint.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"

namespace game
{
    bloodprint::bloodprint()
    {
        
    }
    
    bloodprint::~bloodprint()
    {
        
    }
    
    void bloodprint::setup(const std::string& filename,
                          const gb::scene_graph_shared_ptr& scene_graph,
                          const gb::scene_fabricator_shared_ptr& scene_fabricator,
                          const gb::anim::anim_fabricator_shared_ptr& anim_fabricator)
    {
        auto bloodprint = anim_fabricator->create_animated_sprite(filename, "bloodprint");
        bloodprint->tag = "bloodprint";
        bloodprint->goto_and_play(0);
        bloodprint->is_luminous = true;
        
        bloodprint::add_child(bloodprint);
    }
}
