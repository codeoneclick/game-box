//
//  ces_level_layers_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_level_layers_component.h"

namespace game
{
    const std::string ces_level_layers_component::k_layer_default = "layer_default";
    const std::string ces_level_layers_component::k_layer_bloodprints = "layer_bloodprints";
    const std::string ces_level_layers_component::k_layer_footprints = "layer_footprints";
    const std::string ces_level_layers_component::k_layer_hit_bounds = "layer_hit_bounds";
    const std::string ces_level_layers_component::k_layer_characters_down_statistic = "layer_characters_down_statistic";
    const std::string ces_level_layers_component::k_layer_characters = "layer_characters";
    const std::string ces_level_layers_component::k_layer_characters_top_statistic = "layer_characters_top_statistic";
    
    ces_level_layers_component::ces_level_layers_component()
    {

    }
    
    ces_level_layers_component::~ces_level_layers_component()
    {
        
    }
    
    const std::array<gb::ces_entity_weak_ptr, ces_level_layers_component::e_level_layer_max>& ces_level_layers_component::get_layers()
    {
        return m_layers;
    }
    
    void ces_level_layers_component::set_layer(const gb::ces_entity_shared_ptr& entity, e_level_layer layer)
    {
        m_layers[layer] = entity;
    }
}
