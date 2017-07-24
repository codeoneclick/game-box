//
//  scene_graph.h
//  gbCore
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_entity.h"

namespace gb
{
    class scene_graph : public ces_entity
    {
    private:
        
        std::weak_ptr<game_transition> m_transition;
        scene_fabricator_shared_ptr m_fabricator;
        
        static void disassembly_scene_recursively(const ces_entity_shared_ptr& entity,
                                                  std::map<f32, std::string>& scene_graph_metadata);
        
        
    protected:
        
        game_transition_shared_ptr get_transition() const;
        void updated_z_order_recursively(const ces_entity_shared_ptr& entity, f32& z_order);
        
        game_commands_container_shared_ptr m_internal_commands;
        game_commands_container_shared_ptr m_external_commands;
        
        camera_2d_shared_ptr m_camera_2d;
        camera_3d_shared_ptr m_camera_3d;
        
        glm::vec2 m_scene_size;
        
    public:
        
        scene_graph(const game_transition_shared_ptr& transition);
        virtual ~scene_graph();
        
        void set_fabricator(const scene_fabricator_shared_ptr& fabricator);
        scene_fabricator_shared_ptr get_fabricator() const;
        
        virtual void create();
        
        void set_external_commands(const game_commands_container_shared_ptr& commands);
        game_commands_container_shared_ptr get_internal_commands() const;
        
        void set_camera_2d(const camera_2d_shared_ptr& camera);
        camera_2d_shared_ptr get_camera_2d() const;
        
        void set_camera_3d(const camera_3d_shared_ptr& camera);
        camera_3d_shared_ptr get_camera_3d() const;
        
        void add_child(const ces_entity_shared_ptr& child) override;
        
        void enable_box2d_world(const glm::vec2& min_bound, const glm::vec2& max_bound);
        void disable_box2d_world();
        
        void set_systems_feeder(const ces_systems_feeder_shared_ptr& systems_feeder);
        
        static std::map<f32, std::string> disassembly_scene(const ces_entity_shared_ptr& root);
    };
};
