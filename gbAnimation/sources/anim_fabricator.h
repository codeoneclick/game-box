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

namespace gb
{
    namespace anim
    {
        class animated_sprite;
        class anim_fabricator
        {
        private:
            
        protected:
            
            scene_fabricator_shared_ptr m_fabricator;
            std::set<game_object_shared_ptr> m_game_objects_container;
            
        public:
            
            anim_fabricator(const scene_fabricator_shared_ptr& fabricator);
            ~anim_fabricator();
            
            std::shared_ptr<animated_sprite> create_animated_sprite(const std::string& filename);
        };
    };
};

#endif
