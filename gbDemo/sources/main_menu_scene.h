//
//  main_menu_scene.h
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace game
{
    class main_menu_scene : public gb::scene_graph
    {
    private:
        
        //enum e_mode
        //{
        //    e_mode_main_menu = 0,
        //    e_mode_in_game,
        //};
        
        //bool m_is_scene_loading = false;
        //bool m_is_scene_loaded = false;
        //f32 m_scene_loading_progress = 0.f;
        //f32 m_scene_loading_interval = .05f;
        
        //void init_scene_as_main_menu(const std::string& filename);
        //void init_scene_as_in_game(const std::string& filename);
        //void de_init();
        
        //void place_car_on_level(const gb::game_object_3d_shared_ptr& car, const glm::vec2& spawner_position);
        
    protected:
        
        //e_mode m_mode = e_mode_main_menu;
        
        gb::ui::ui_fabricator_shared_ptr m_ui_base_fabricator = nullptr;
        
		gameplay_fabricator_shared_ptr m_gameplay_fabricator = nullptr;;
        gameplay_ui_fabricator_shared_ptr m_gameplay_ui_fabricator = nullptr;

        gb::camera_3d_shared_ptr m_camera_3d;
        
        //gb::game_object_3d_shared_ptr m_level = nullptr;
        //gb::game_object_3d_shared_ptr m_car = nullptr;
        //std::array<gb::game_object_3d_shared_ptr, 3> m_opponents;
        
		void on_goto_ui_editor_scene(gb::ces_entity_const_shared_ptr entity);
        void on_goto_in_game_scene(gb::ces_entity_const_shared_ptr entity);
        void on_play_rewarded_video(gb::ces_entity_const_shared_ptr entity);
        void on_goto_net_menu_scene(gb::ces_entity_const_shared_ptr entity);
        
        void on_update(gb::ces_entity_const_shared_ptr entity, f32 dt);
        
    public:
        
        main_menu_scene(const gb::game_transition_shared_ptr& transition);
        ~main_menu_scene();
        
        void create() override;
    };
};
