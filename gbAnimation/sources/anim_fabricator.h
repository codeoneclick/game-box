//
//  anim_fabricator.hpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "tags_defines.h"
#include "ani_declarations.h"

namespace gb
{
    forward_decl(anim_configuration_accessor)
    
    namespace anim
    {
        class anim_fabricator
        {
        private:
            
        protected:
            
            scene_fabricator_weak_ptr m_general_fabricator;
            anim_configuration_accessor_shared_ptr m_anim_configuration_accessor;
            
#if USED_GRAPHICS_API != NO_GRAPHICS_API
            
            bool is_using_batch(const std::vector<std::shared_ptr<configuration>>& configurations);
            
            void apply_materials_recursively(const ces_entity_shared_ptr& entity,
                                             const std::vector<std::shared_ptr<configuration>>& configurations);
            
#endif
            
            void create_animated_objects_from_timeline(const std::shared_ptr<animated_sprite>& animated_object,
                                                       const std::shared_ptr<ani_asset_metadata>& metadata,
                                                       const std::shared_ptr<ani_timeline>& timeline,
                                                       bool is_using_batch);
            
            void create_animated_subobject(const std::shared_ptr<animated_sprite>& animated_object,
                                           const std::shared_ptr<ani_timeline>& timeline,
                                           const std::shared_ptr<ani_asset_metadata>& metadata,
                                           ui32 id, e_ani_character_type character_type, ui32 reference, bool is_mask,
                                           bool is_using_batch);
            
        public:
            
            anim_fabricator(const scene_fabricator_shared_ptr& general_fabricator);
            ~anim_fabricator();
            
            std::shared_ptr<animated_sprite> create_animated_sprite(const std::string& filename, const std::string& timeline_name);
        };
    };
};
