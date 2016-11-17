#pragma once

#include "ani_tag_parser_base.h"

namespace gb
{
    namespace anim
    {
        class ani_subobject_state;
        
        class ani_tag_parser_animation_frames : public ani_tag_parser_base
        {
        protected:
            
            typedef std::map<ui32, std::shared_ptr<ani_subobject_state>> ani_states_t;
            virtual std::shared_ptr<ani_subobject_state> extract_state(const std::shared_ptr<ani_stream>& stream);
            
        public:
            
            virtual void read(const ani_parser_t& parser,
                              const std::shared_ptr<ani_stream>& stream,
                              const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                              const std::shared_ptr<ani_timeline>& timeline) override;
        };
    };
};
