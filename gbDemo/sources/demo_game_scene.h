//
//  demo_game_scene.h
//  gbDemo
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef demo_game_scene_h
#define demo_game_scene_h

#include "game_scene.h"
#include "declarations.h"
#include "ui_declarations.h"
#include "ces_input_system.h"
#include "koth_declarations.h"

class demo_game_scene : public gb::game_scene, public gb::touch_listener
{
private:
    
    gb::camera_shared_ptr m_camera;
    gb::shadow_cast_light_shared_ptr m_shadow_cast_light;
    gb::direction_light_shared_ptr m_direction_light;
    std::map<std::string, gb::model3d_animated_shared_ptr> m_models;
    std::map<std::string, gb::omni_light_shared_ptr> m_omni_lights;
    gb::model3d_static_shared_ptr m_grid;
    gb::model3d_static_shared_ptr m_3d_tile_cursor;
    //gb::instanced_omni_lights_shared_ptr m_instanced_omni_lights;
    gb::skybox_shared_ptr m_skybox;
    gb::ocean_shared_ptr m_ocean;
    
    koth::level_shared_ptr m_level;
    koth::character_controller_shared_ptr m_character_controller;
    std::map<std::string, koth::ai_character_controller_shared_ptr> m_ai_character_controllers;
    
    gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
    gb::ui::ui_graph_shared_ptr m_ui_graph;
    
protected:
    
    void update(f32 deltatime);
    
    void on_touch(const glm::vec3& point, const glm::vec3& delta, const gb::ces_entity_shared_ptr& listener,
                  gb::e_input_element input_element, gb::e_input_state input_state);
    
    void on_key_down(i32 key);
    void on_key_up(i32 key);
    
    void on_move_state_changed(i32 state);
    void on_rotate_state_changed(i32 state);
    void on_mouse_dragged(const glm::ivec2& delta);
    
    void on_use_ability(i32 ability_id);
    
public:
    
    demo_game_scene(const gb::game_transition_shared_ptr& transition);
    ~demo_game_scene();
};

#endif
