//
//  anim_fabricator.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "anim_fabricator.h"
#include "scene_fabricator.h"
#include "resource_accessor.h"
#include "animated_sprite.h"
#include "animation.h"
#include "animation_loading_operation.h"
#include "ani_asset_metadata.h"

namespace gb
{
    namespace anim
    {
        anim_fabricator::anim_fabricator(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator)
        {
            
        }
        
        anim_fabricator::~anim_fabricator()
        {
            
        }
        
        std::shared_ptr<animated_sprite> anim_fabricator::create_animated_sprite(const std::string& filename)
        {
             std::shared_ptr<gb::anim::animation> animation = m_fabricator->get_resource_accessor()->get_resource<gb::anim::animation, gb::anim::animation_loading_operation>(filename, true);
            
            return nullptr;
        }
    };
};
