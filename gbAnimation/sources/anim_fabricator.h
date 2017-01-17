//
//  anim_fabricator.hpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef anim_fabricator_h
#define anim_fabricator_h

#include "main_headers.h"
#include "declarations.h"
#include "tags_defines.h"

namespace gb
{
    class anim_configuration_accessor;
    namespace anim
    {
        class ani_timeline;
        class animated_sprite;
        class ani_asset_metadata;
        class anim_fabricator
        {
        private:
            
        protected:
            
            scene_fabricator_shared_ptr m_fabricator;
            std::set<game_object_shared_ptr> m_game_objects_container;
            std::shared_ptr<anim_configuration_accessor> m_anim_configuration_accessor;
            
#if !defined(__NO_RENDER__)

            void apply_materials_recursively(const ces_entity_shared_ptr& entity,
                                             const std::vector<std::shared_ptr<configuration>>& configurations);

#endif
            
            void create_animated_objects_from_timeline(const std::shared_ptr<animated_sprite>& animated_object,
                                                       const std::shared_ptr<ani_asset_metadata>& metadata,
                                                       const std::shared_ptr<ani_timeline>& timeline);
            
            void create_animated_subobject(const std::shared_ptr<animated_sprite>& animated_object,
                                           const std::shared_ptr<ani_timeline>& timeline,
                                           const std::shared_ptr<ani_asset_metadata>& metadata,
                                           ui32 id, e_ani_character_type character_type, ui32 reference, bool is_mask);
            
        public:
            
            anim_fabricator(const scene_fabricator_shared_ptr& fabricator);
            ~anim_fabricator();
            
            std::shared_ptr<animated_sprite> create_animated_sprite(const std::string& filename, const std::string& timeline_name);
        };
    };
};

#endif
