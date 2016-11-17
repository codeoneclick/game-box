//
//  animated_sprite.hpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef animated_sprite_h
#define animated_sprite_h

#include "sprite.h"

namespace gb
{
    namespace anim
    {
        class animated_sprite : public sprite
        {
        private:
            
        protected:
            
            std::shared_ptr<animated_sprite> get_named_part_recursively(const ces_entity_shared_ptr& entity,
                                                                        const std::string& name,
                                                                        i32& object_id_refernce);
            
        public:
            
            animated_sprite();
            ~animated_sprite();
            
            std::weak_ptr<animated_sprite> get_named_part(const std::string& name);
            
            void goto_and_stop(i32 frame_index);
        };
    };
};

#endif
