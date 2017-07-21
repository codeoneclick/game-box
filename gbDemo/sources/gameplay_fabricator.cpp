//
//  gameplay_fabricator.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "gameplay_fabricator.h"
#include "gameplay_configuration_accessor.h"
#include "characters_3d_assembler.h"
#include "scene_fabricator.h"

namespace game
{
    gameplay_fabricator::gameplay_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                                             const gb::anim::anim_fabricator_shared_ptr& anim_fabricator) :
    m_general_fabricator(general_fabricator),
    m_anim_fabricator(anim_fabricator)
    {
        m_gameplay_configuration_accessor = std::make_shared<gb::gameplay_configuration_accessor>();
        m_characters_3d_assembler = std::make_shared<characters_3d_assembler>();
    }
    
    gb::ces_entity_shared_ptr gameplay_fabricator::create_character(const std::string& filename)
    {
        auto character_configuration = std::static_pointer_cast<gb::character_configuration>(m_gameplay_configuration_accessor->get_character_configuration(filename));
        auto character_main_3d_entity = m_general_fabricator.lock()->create_shape_3d(character_configuration->get_main_3d_configuration_filename());
        auto character_main_2d_entity = m_general_fabricator.lock()->create_sprite(character_configuration->get_main_2d_configuration_filename());
        auto character_avatar_3d_entity = m_general_fabricator.lock()->create_shape_3d(character_configuration->get_avatar_3d_configuration_filename());
        auto character_avatar_2d_entity = m_general_fabricator.lock()->create_sprite(character_configuration->get_avatar_2d_configuration_filename());
        
        return nullptr;
    }
    
    gb::ces_entity_shared_ptr gameplay_fabricator::create_mob(const std::string& filename)
    {
        return nullptr;
    }
}
