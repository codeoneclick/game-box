//
//  ability_button.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object_2d.h"
#include "input_context.h"

namespace game
{
    class ability_button : public gb::game_object_2d
    {
    public:
        
        static const std::string k_pressed_state;
        static const std::string k_released_state;
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&)> on_tap_on_ability_callback_t;
        
    private:

        static const std::string k_background_entity_filename;
        static const f32 k_background_size;
        
        std::unordered_map<std::string, std::string> m_sounds_linkage;
        
        on_tap_on_ability_callback_t m_on_tap_on_ability_callback;
        
        void on_touched(const gb::ces_entity_shared_ptr&, const glm::vec2& point,
                        gb::e_input_source input_source,
                        gb::e_input_state input_state);
        
    protected:
        
    public:
        
        ability_button();
        ~ability_button();
        
        void setup(const std::string& filename,
                   const gb::scene_graph_shared_ptr& scene_graph,
                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                   const gb::anim::anim_fabricator_shared_ptr& anim_fabricator);
        
        void attach_sound(const std::string& filename, const std::string& state);
        
        void set_on_tap_on_ability_callback(const on_tap_on_ability_callback_t& callback);
    };
};
