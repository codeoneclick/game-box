//
//  gameplay_fabricator.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "tags_defines.h"
#include "ani_declarations.h"
#include "ns_declarations.h"

namespace gb
{
    forward_decl(gameplay_configuration_accessor)
};


namespace game
{
    class gameplay_fabricator
    {
    private:
        
        characters_3d_assembler_shared_ptr m_characters_3d_assembler;
        
    protected:
        
        gb::scene_fabricator_weak_ptr m_general_fabricator;
        gb::anim::anim_fabricator_weak_ptr m_anim_fabricator;
        gb::gameplay_configuration_accessor_shared_ptr m_gameplay_configuration_accessor;
        
    public:
        
        gameplay_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                            const gb::anim::anim_fabricator_shared_ptr& anim_fabricator);
        ~gameplay_fabricator() = default;
        
        gb::ces_entity_shared_ptr create_character(const std::string& filename);
        gb::ces_entity_shared_ptr create_mob(const std::string& filename);
    };
};

