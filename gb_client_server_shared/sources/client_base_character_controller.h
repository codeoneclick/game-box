//
//  client_base_character_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object.h"
#include "level.h"

namespace game
{
    class client_base_character_controller : public gb::game_object
    {
    private:
        
    protected:
        
        gb::scene_graph_weak_ptr m_scene_graph;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        gb::anim::anim_fabricator_weak_ptr m_anim_fabricator;
        
        gb::game_object_shared_ptr m_character;
        std::array<gb::game_object_weak_ptr, level::e_level_layer_max> m_layers;
        
        glm::vec2 m_server_velocity;
        glm::vec2 m_server_position;
        f32 m_server_rotation;
        
        virtual void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        client_base_character_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                         const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                         const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                         const std::array<gb::game_object_weak_ptr, level::e_level_layer_max>& layers);
        ~client_base_character_controller();
        
        void setup(const std::string& filename);
        
        void on_changed_server_transformation(const glm::vec2& velocity,
                                              const glm::vec2& position,
                                              f32 rotation);
    };
};
