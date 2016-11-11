#pragma once

#include "ani_tag_parser_animation_frames.h"

namespace gb
{
    namespace anim
    {
        class ani_subobject_state;
        
        class ani_tag_parser_animation_frames2 : public ani_tag_parser_animation_frames
        {
            
        public:
            
            virtual void read(const ani_parser_t& parser,
                              const std::shared_ptr<ani_stream>& stream,
                              const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                              const std::shared_ptr<ani_timeline>& timeline) override;
        };
    };
};
