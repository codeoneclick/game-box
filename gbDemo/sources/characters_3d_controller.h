//
//  characters_3d_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 5/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "ces_entity.h"

namespace game
{
    class characters_3d_controller : public gb::ces_entity
    {
    private:
        
        gb::camera_3d_shared_ptr m_camera_3d;
        gb::scene_graph_weak_ptr m_scene_graph;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        
    protected:
        
        std::map<gb::ces_entity_shared_ptr, gb::ces_entity_shared_ptr> m_characters_3d_container;
        
    public:
        
        characters_3d_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                 const gb::scene_fabricator_shared_ptr& scene_fabricator);
        ~characters_3d_controller();
        
        std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr> create_character(const std::string& character_sprite_filename,
                                                                                   const std::string& character_shape_3d_filename,
                                                                                   const glm::vec2& size);
        void remove_character(const gb::ces_entity_shared_ptr& character);
    };
};