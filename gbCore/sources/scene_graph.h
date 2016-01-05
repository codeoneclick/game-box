//
//  scene_graph.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef scene_graph_h
#define scene_graph_h

#include "main_headers.h"
#include "declarations.h"
#include "i_scene_graph.h"
#include "scene_graph_parameters.h"

#define scene_graph_id 0
#define scene_graph_inst std::static_pointer_cast<gb::scene_graph>(game_scene::get_transition()->get_graph(scene_graph_id))

namespace gb
{
    class scene_graph : public i_scene_graph, public scene_graph_parameters,
    public std::enable_shared_from_this<scene_graph>
    {
    private:
        
    protected:
        
        void on_update(f32 deltatime);
        
    public:
        
        scene_graph();
        ~scene_graph();
        
        void set_main_camera(const camera_shared_ptr& camera);
        
        void set_shadow_cast_light(f32 fov, f32 near, f32 far);
        shadow_cast_light_shared_ptr get_shadow_cast_light() const;
        
        void add_direction_light(const direction_light_shared_ptr& direction_light);
        void remove_direction_light(const direction_light_shared_ptr& direction_light);
        
        void add_omni_light(const omni_light_shared_ptr& omni_light);
        void remove_omni_light(const omni_light_shared_ptr& omni_light);
        
        void add_game_object(const game_object_shared_ptr& game_object);
        void remove_game_object(const game_object_shared_ptr& game_object);
        
        void set_box2d_world(const glm::vec2 &min_bound, const glm::vec2 &max_bound);
        
        b2Body* add_box2d_body(const std::shared_ptr<b2BodyDef> box2d_body_definition);
        void remove_box2d_body(b2Body* box2d_body);
    };
};

#endif
