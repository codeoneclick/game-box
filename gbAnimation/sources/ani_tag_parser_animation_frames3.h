#pragma once

#include "ani_tag_parser_animation_frames2.h"

namespace gb
{
    namespace anim
    {
        class ani_subobject_state;
        
        class ani_tag_parser_animation_frames3 : public ani_tag_parser_animation_frames2
        {
            
        public:
            
            std::shared_ptr<ani_subobject_state> extract_state(const std::shared_ptr<ani_stream>& stream) override;
        };
    };
};
