//
//  ces_level_layers_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_level_layers_component : public gb::ces_base_component
    {
    public:
        
        enum e_level_layer
        {
            e_level_layer_default = 0,
            e_level_layer_bloodprints,
            e_level_layer_footprints,
            e_level_layer_hit_bounds,
            e_level_layer_characters_down_statistic,
            e_level_layer_characters,
            e_level_layer_characters_top_statistic,
            e_level_layer_max
        };
        
        static const std::string k_layer_default;
        static const std::string k_layer_bloodprints;
        static const std::string k_layer_footprints;
        static const std::string k_layer_hit_bounds;
        static const std::string k_layer_characters_down_statistic;
        static const std::string k_layer_characters;
        static const std::string k_layer_characters_top_statistic;
        
    private:
        
         std::array<gb::ces_entity_weak_ptr, e_level_layer_max> m_layers;
        
    protected:

        
    public:
        
        CTTI_CLASS_GUID(ces_level_layers_component, gb::ces_base_component::g_guids_container)
        ces_level_layers_component();
        ~ces_level_layers_component();
        
        const std::array<gb::ces_entity_weak_ptr, e_level_layer_max>& get_layers();
        void set_layer(const gb::ces_entity_shared_ptr& entity, e_level_layer layer);
    };
};


