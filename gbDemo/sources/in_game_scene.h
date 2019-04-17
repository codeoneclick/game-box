//
//  in_game_scene.h
//  gbDemo
//
//  Created by serhii.s on 4/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace game
{
    class in_game_scene : public gb::scene_graph
    {
    private:
        
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_base_fabricator = nullptr;
        gameplay_fabricator_shared_ptr m_gameplay_fabricator = nullptr;;
        gameplay_ui_fabricator_shared_ptr m_gameplay_ui_fabricator = nullptr;;
        
        gb::game_object_3d_shared_ptr m_car = nullptr;
        
    public:
        
        in_game_scene(const gb::game_transition_shared_ptr& transition);
        ~in_game_scene() = default;
        
        void create() override;
    };
};

