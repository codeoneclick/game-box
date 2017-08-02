//
//  in_game_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "local_session_game_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "gameplay_fabricator.h"
#include "gameplay_ui_fabricator.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "button.h"
#include "joystick.h"
#include "textfield.h"
#include "action_console.h"
#include "game_commands_container.h"
#include "ces_box2d_body_component.h"
#include "ns_ui_commands.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source_2d.h"
#include "ces_ani_animation_system.h"
#include "ces_ui_interaction_system.h"
#include "ces_character_visibility_system.h"
#include "ces_deferred_lighting_system.h"
#include "ces_state_automat_system.h"
#include "ces_character_visual_effects_system.h"
#include "ces_transformation_2d_component.h"
#include "ces_battle_system.h"
#include "ces_sound_system.h"
#include "ces_interaction_system.h"
#include "vbo.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_extension.h"
#include "ces_ai_system.h"
#include "path_map.h"
#include "label.h"
#include "shape_3d.h"
#include "ces_level_layers_component.h"
#include "ces_level_path_grid_component.h"
#include "db_declarations.h"

namespace game
{
    local_session_game_scene::local_session_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
    }
    
    local_session_game_scene::~local_session_game_scene()
    {
        
    }
    
    void local_session_game_scene::create()
    {
        gb::scene_graph::create();
        
        m_scene_size = glm::vec2(667, 375);
        
        auto character_visibility_system = std::make_shared<ces_character_visibility_system>();
        character_visibility_system->set_order(1);
        local_session_game_scene::get_transition()->add_system(character_visibility_system);
        
        auto battle_system = std::make_shared<ces_battle_system>();
        battle_system->set_order(2);
        local_session_game_scene::get_transition()->add_system(battle_system);
        
        auto ai_system = std::make_shared<ces_ai_system>();
        local_session_game_scene::get_transition()->add_system(ai_system);
        
        auto sound_system = std::make_shared<gb::al::ces_sound_system>();
        local_session_game_scene::get_transition()->add_system(sound_system);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
        animation_system->set_order(3);
        local_session_game_scene::get_transition()->add_system(animation_system);
        
        auto interaction_system = std::make_shared<ces_interaction_system>();
        local_session_game_scene::get_transition()->add_system(interaction_system);
        
        auto ui_interaction_system = std::make_shared<ces_ui_interaction_system>();
        local_session_game_scene::get_transition()->add_system(ui_interaction_system);
        
        auto state_automat_system = std::make_shared<ces_state_automat_system>();
        local_session_game_scene::get_transition()->add_system(state_automat_system);
        
        auto character_visual_effects_system = std::make_shared<ces_character_visual_effects_system>();
        local_session_game_scene::get_transition()->add_system(character_visual_effects_system);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(local_session_game_scene::get_fabricator());
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(local_session_game_scene::get_fabricator());
        m_gameplay_fabricator = std::make_shared<gameplay_fabricator>(local_session_game_scene::get_fabricator(),
                                                                      m_anim_fabricator);
        m_gameplay_ui_fabricator = std::make_shared<gameplay_ui_fabricator>(local_session_game_scene::get_fabricator(),
                                                                            m_anim_fabricator,
                                                                            m_ui_fabricator);
        
        m_camera_2d = std::make_shared<gb::camera_2d>(m_scene_size.x,
                                                      m_scene_size.y);
        local_session_game_scene::set_camera_2d(m_camera_2d);
        
        auto camera_3d = std::make_shared<gb::camera_3d>(90.f, .1f, 1024.f,
                                                         glm::ivec4(0,
                                                                    0,
                                                                    16,
                                                                    16), false);
        camera_3d->set_distance_to_look_at(glm::vec3(8.f, 0.f, 0.f));
        local_session_game_scene::set_camera_3d(camera_3d);
        
        auto level = m_gameplay_fabricator->create_level("level.village.xml");
        local_session_game_scene::add_child(level);
        
        auto level_path_grid_component = level->get_component<ces_level_path_grid_component>();
        auto level_size = level_path_grid_component->get_level_size();
        local_session_game_scene::enable_box2d_world(glm::vec2(0.f, 0.f),
                                                     glm::vec2(level_size.x, level_size.y));
        
        auto level_layers_component = level->get_component<ces_level_layers_component>();
        auto layers = level_layers_component->get_layers();
        
        auto character = m_gameplay_fabricator->create_character("character.knight.xml", layers);
        layers[ces_level_layers_component::e_level_layer_characters].lock()->add_child(character);
        character->position = glm::vec2(128.f , 128.f);
        
        auto mob_ghoul = m_gameplay_fabricator->create_mob("mob.ghoul.xml", layers);
        layers[ces_level_layers_component::e_level_layer_characters].lock()->add_child(mob_ghoul);
        mob_ghoul->position = glm::vec2(512.f , 128.f);
        
        auto npc_orc = m_gameplay_fabricator->create_npc("npc.orc.xml", layers);
        layers[ces_level_layers_component::e_level_layer_characters].lock()->add_child(npc_orc);
        npc_orc->position = glm::vec2(128.f , 512.f);
        
        auto attack_button = m_gameplay_ui_fabricator->create_attack_button("ui.attack.button.xml");
        attack_button->position = glm::vec2(16.f, m_scene_size.y - 80.f);
        local_session_game_scene::add_child(attack_button);
        
        auto character_avatar_icon = m_gameplay_ui_fabricator->create_character_avatar_icon("ui.character.avatar.icon.xml");
        character_avatar_icon->position = glm::vec2(8.f);
        local_session_game_scene::add_child(character_avatar_icon);
        
        auto opponent_avatar_icon = m_gameplay_ui_fabricator->create_opponent_avatar_icon("ui.opponent.avatar.icon.xml");
        opponent_avatar_icon->position = glm::vec2(m_scene_size.x - 88.f, 8.f);
        local_session_game_scene::add_child(opponent_avatar_icon);
        
        auto quest_dialog = m_gameplay_ui_fabricator->create_quest_dialog("quest.dialog.xml");
        local_session_game_scene::add_child(quest_dialog);
        
    }
}
