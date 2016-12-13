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
    public:
        
        typedef std::function<void(ces_box2d_body_component_const_shared_ptr component)> custom_setup_box2d_component_t;
        
    private:
        
        std::weak_ptr<game_transition> m_transition;
        scene_fabricator_shared_ptr m_fabricator;
        
    protected:
        
        game_transition_shared_ptr get_transition() const;
        void updated_z_order_recursively(const ces_entity_shared_ptr& entity, f32& z_order);
        
        game_commands_container_shared_ptr m_internal_commands;
        game_commands_container_shared_ptr m_external_commands;
        
        camera_shared_ptr m_camera;
    
    public:
        
        scene_graph(const game_transition_shared_ptr& transition);
        virtual ~scene_graph();
        
        void set_fabricator(const scene_fabricator_shared_ptr& fabricator);
        scene_fabricator_shared_ptr get_fabricator() const;
        
        virtual void create() = 0;
        
        void set_external_commands(const game_commands_container_shared_ptr& commands);
        game_commands_container_shared_ptr get_internal_commands() const;
        
        void set_camera(const camera_shared_ptr& camera);
        camera_shared_ptr get_camera() const;
        
        void add_child(const ces_entity_shared_ptr& child) override;
        
        void enable_box2d_world(const glm::vec2& min_bound, const glm::vec2& max_bound);
        void disable_box2d_world();
        
        void apply_box2d_physics(const ces_entity_shared_ptr& entity, const custom_setup_box2d_component_t& callback = nullptr);
        void remove_box2d_physics(const ces_entity_shared_ptr& entity);
    };
};

#endif
